#include "MultiGameViewController.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::pair;
using std::string;
using std::vector;

MultiGameViewController::MultiGameViewController() : GameViewController() {
    _initGL();
}

MultiGameViewController::~MultiGameViewController() {
}

void MultiGameViewController::process() {
    GameViewController::process();
}

void MultiGameViewController::_initGL() {
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

    // context = cl::Context(devices, NULL, NULL, NULL, NULL);
    // devices = context.getInfo<CL_CONTEXT_DEVICES>();

    if (devices.size() <= 0) {
        cout << "No OpenCL devices found. Quitting." << endl;
        exit(1);
    }
    cout << "Number of OpenCL devices found: " << devices.size() << endl;
    for (unsigned i = 0; i < devices.size(); i++) {
        cout << "Device " << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>()
             << " Vendor ID: " << devices[i].getInfo<CL_DEVICE_VENDOR_ID>()
             << endl;
        vector<cl::Device> temp;
        temp.push_back(devices[i]);
        deviceContexts.push_back(cl::Context(temp, NULL, NULL, NULL, NULL));
    }

    string clCode;
    {
        ifstream code("MultiRender.cl");
        clCode = string((std::istreambuf_iterator<char>(code)),
                        std::istreambuf_iterator<char>());
    }

    cl::Program::Sources clSource(
        1, pair<const char *, int>(clCode.c_str(), clCode.length() + 1));
    for (unsigned i = 0; i < devices.size(); i++) {
        devicePrograms.push_back(cl::Program(deviceContexts[i], clSource));
        if (devicePrograms[i].build(
                deviceContexts[i].getInfo<CL_CONTEXT_DEVICES>(), "-I.")) {
            cout << devicePrograms[i].getBuildInfo<CL_PROGRAM_BUILD_LOG>(
                        devices[i]) << endl;
            exit(1);
        }
    }
    /*
    if (program.build(devices, "-I.")) {
            cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) <<
    endl;
            exit(1);
    }
    */

    glViewport(0, 0, WIDTH, HEIGHT);
    glOrtho(-1, 1, -1, 1, -1, 1);
    glEnable(GL_TEXTURE_2D);
    glGenTextures((GLsizei)devices.size(), &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    pixels = new int[WIDTH * HEIGHT];
    for (unsigned i = 0; i < devices.size(); i++) {
        int render_offset = (((i + 1) * (HEIGHT / 16)) / devices.size() -
                             (i * (HEIGHT / 16)) / devices.size()) *
                            16;
        images.push_back(cl::Image2D(deviceContexts[i], CL_MEM_WRITE_ONLY,
                                     cl::ImageFormat(CL_RGBA, CL_UNORM_INT8),
                                     WIDTH, render_offset));
        cqs.push_back(cl::CommandQueue(deviceContexts[i], devices[i], 0, NULL));
    }

    glDisable(GL_TEXTURE_2D);
}

void MultiGameViewController::render() {
    glDisable(GL_DEPTH_TEST);
    if (world->getMyObject() == NULL) {
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    map<int, MovingRoundObject *> const &roundObjects =
        world->getMovingRoundObjects();

    float focusx = world->getMyObject()->center.x,
          focusy = world->getMyObject()->center.y;
    if (focusx < world->getMinX() + 14)
        focusx += (14 - focusx + world->getMinX()) *
                  (14 - focusx + world->getMinX()) / 28.0;
    if (focusx > world->getMaxX() - 14)
        focusx -= (14 + focusx - world->getMaxX()) *
                  (14 + focusx - world->getMaxX()) / 28.0;
    if (focusy < world->getMinY() + 14)
        focusy += (14 - focusy + world->getMinY()) *
                  (14 - focusy + world->getMinY()) / 28.0;
    if (focusy > world->getMaxY() - 14)
        focusy -= (14 + focusy - world->getMaxY()) *
                  (14 + focusy - world->getMaxY()) / 28.0;

    map<int, RectangularWall *> const &rectangularWalls =
        world->getRectangularWalls();

    float obspoints[4 * rectangularWalls.size()];
    unsigned char obscolor[4 * rectangularWalls.size()];
    unsigned ti, i2 = 0;
    for (ti = 0;; ti++) {
        while (i2 != rectangularWalls.size()) {
            RectangularWall *wall = rectangularWalls.find(i2)->second;
            Vector2D diff = Vector2D(focusx, focusy) - wall->p1;
            Vector2D obsdir = wall->p2 - wall->p1;
            float smallest;
            if (diff * obsdir <= 0)
                smallest = diff * diff;
            else if (diff * obsdir >= obsdir * obsdir)
                smallest = (diff - obsdir) * (diff - obsdir);
            else
                smallest =
                    diff * diff -
                    (diff * obsdir) * (diff * obsdir) / (obsdir * obsdir);
            if (smallest <= 56 * 56 * (1 +
                                       float(WIDTH) * float(WIDTH) /
                                           float(HEIGHT) / float(HEIGHT) / 4.0))
                break;
            i2++;
        }
        if (i2 == rectangularWalls.size())
            break;
        RectangularWall *wall = rectangularWalls.find(i2)->second;
        obspoints[4 * ti] = wall->p1.x;
        obspoints[4 * ti + 1] = wall->p1.y;
        obspoints[4 * ti + 2] = wall->p2.x;
        obspoints[4 * ti + 3] = wall->p2.y;
        obscolor[4 * ti] = wall->getMaterial()->getR();
        obscolor[4 * ti + 1] = wall->getMaterial()->getG();
        obscolor[4 * ti + 2] = wall->getMaterial()->getB();
        obscolor[4 * ti + 3] = wall->getMaterial()->getA();
        i2++;
    }
    vector<cl::Buffer> obspointsbuf, obscolorbuf, objpointbuf, objsizebuf,
        objcolorbuf, lightposbuf, lightcolorbuf;
    for (unsigned i = 0; i < devices.size(); i++) {
        obspointsbuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            4 * rectangularWalls.size() * sizeof(float), obspoints, NULL));
        obscolorbuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            4 * rectangularWalls.size() * sizeof(char), obscolor, NULL));
    }

    float objpoint[2 * roundObjects.size()];
    float objsize[2 * roundObjects.size()];
    unsigned char objcolor[4 * roundObjects.size()];
    auto it = roundObjects.begin();
    int si;
    for (si = 0;; si++) {
        while (it != roundObjects.end() &&
               (focusx - it->second->center.x) *
                           (focusx - it->second->center.x) +
                       (focusy - it->second->center.y) *
                           (focusx - it->second->center.y) >
                   (56 * sqrt(1 +
                              float(WIDTH) * float(WIDTH) / float(HEIGHT) /
                                  float(HEIGHT) / 4.0) +
                    it->second->radius) *
                       (56 * sqrt(1 +
                                  float(WIDTH) * float(WIDTH) / float(HEIGHT) /
                                      float(HEIGHT) / 4.0) +
                        it->second->radius))
            it++;
        if (it == roundObjects.end())
            break;
        objpoint[2 * si] = it->second->center.x;
        objpoint[2 * si + 1] = it->second->center.y;
        objsize[2 * si] = it->second->radius;
        objsize[2 * si + 1] = it->second->heightRatio;
        MaterialPtr color = it->second->getMaterial();
        objcolor[4 * si] = color->getR();
        objcolor[4 * si + 1] = color->getG();
        objcolor[4 * si + 2] = color->getB();
        objcolor[4 * si + 3] = color->getA();
        it++;
    }
    for (unsigned i = 0; i < devices.size(); i++) {
        objpointbuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            2 * roundObjects.size() * sizeof(float), objpoint, NULL));
        objsizebuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            2 * roundObjects.size() * sizeof(float), objsize, NULL));
        objcolorbuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            4 * roundObjects.size() * sizeof(char), objcolor, NULL));
    }

    float lightpos[3 * world->getLights().size()];
    unsigned char lightcolor[4 * world->getLights().size()];
    for (unsigned i = 0; i < world->getLights().size(); i++) {
        lightpos[3 * i] = world->getLights()[i].getPosition().x;
        lightpos[3 * i + 1] = world->getLights()[i].getPosition().y;
        lightpos[3 * i + 2] = world->getLights()[i].getPosition().z;
        lightcolor[4 * i] = world->getLights()[i].getColor().getR();
        lightcolor[4 * i + 1] = world->getLights()[i].getColor().getG();
        lightcolor[4 * i + 2] = world->getLights()[i].getColor().getB();
        lightcolor[4 * i + 3] = world->getLights()[i].getColor().getA();
    }
    for (unsigned i = 0; i < devices.size(); i++) {
        lightposbuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            3 * world->getLights().size() * sizeof(float), lightpos, NULL));
        lightcolorbuf.push_back(cl::Buffer(
            deviceContexts[i], CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            4 * world->getLights().size() * sizeof(char), lightcolor, NULL));
    }

    for (unsigned i = 0; i < devices.size(); i++) {
        cl::Kernel renderKern(devicePrograms[i], "render", NULL);
        renderKern.setArg(0, focusx - 7 * cosf(angle));
        renderKern.setArg(1, focusy - 7 * sinf(angle));
        renderKern.setArg(2, 4.0f);
        renderKern.setArg(3, cosf(angle));
        renderKern.setArg(4, sinf(angle));
        renderKern.setArg(5, -4.0f / 7);
        renderKern.setArg(6, ti);
        renderKern.setArg(7, obspointsbuf[i]);
        renderKern.setArg(8, obscolorbuf[i]);
        renderKern.setArg(9, si);
        renderKern.setArg(10, objpointbuf[i]);
        renderKern.setArg(11, objsizebuf[i]);
        renderKern.setArg(12, objcolorbuf[i]);
        renderKern.setArg(13, (int)world->getLights().size());
        renderKern.setArg(14, lightposbuf[i]);
        renderKern.setArg(15, lightcolorbuf[i]);
        renderKern.setArg(16, images[i]);
        renderKern.setArg(17, WIDTH);
        renderKern.setArg(18, HEIGHT);
        renderKern.setArg(19, world->getMinX());
        renderKern.setArg(20, world->getMaxX());
        renderKern.setArg(21, world->getMinY());
        renderKern.setArg(22, world->getMaxY());
        renderKern.setArg(23, int(((i * (HEIGHT / 16)) / devices.size()) * 16));
        int render_offset = (((i + 1) * (HEIGHT / 16)) / devices.size() -
                             (i * (HEIGHT / 16)) / devices.size()) *
                            16;
        cqs[i].enqueueNDRangeKernel(renderKern, cl::NullRange,
                                    cl::NDRange(WIDTH, render_offset),
                                    cl::NDRange(16, 16));
        cl::size_t<3> offset, sz;
        sz[0] = WIDTH;
        sz[1] = (((i + 1) * (HEIGHT / 16)) / devices.size() -
                 (i * (HEIGHT / 16)) / devices.size()) *
                16;
        sz[2] = 1;
        cqs[i].enqueueReadImage(
            images[i], false, offset, sz, 0, 0,
            pixels + ((i * (HEIGHT / 16)) / devices.size()) * 16 * WIDTH);
    }
    for (unsigned i = 0; i < devices.size(); i++) cqs[i].finish();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, pixels);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(-1, -1);
    glTexCoord2f(0, 1);
    glVertex2f(-1, 1);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(1, 0);
    glVertex2f(1, -1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    glFlush();

    if (mainmenu->isActive()) {
        mainmenu->drawMenu();
    }
}
