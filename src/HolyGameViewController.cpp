#include "HolyGameViewController.h"

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

HolyGameViewController::HolyGameViewController() : GameViewController() {
	_initGL();
}

HolyGameViewController::~HolyGameViewController() { }

void HolyGameViewController::process() {
	GameViewController::process();
}

void HolyGameViewController::_initGL() {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
#ifdef __APPLE__
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
#endif
	cl_context_properties props[7] = {
		CL_CONTEXT_PLATFORM, 
		(cl_context_properties)(platforms[0])(),
#ifndef __APPLE__
		CL_GLX_DISPLAY_KHR,
		(intptr_t)glXGetCurrentDisplay(), 
		CL_GL_CONTEXT_KHR, 
		(intptr_t)glXGetCurrentContext(),
#else
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties)kCGLShareGroup,
        NULL,
        NULL,
#endif
		0
	};
	context = cl::Context(CL_DEVICE_TYPE_GPU, props, NULL, NULL, NULL);
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	if (devices.size() <=0) {
		cout << "No OpenCL devices found. Quitting." << endl;
		exit(1);
	}
	string clCode;
	{
		ifstream code("Render.cl");
		clCode = string((std::istreambuf_iterator<char>(code)), std::istreambuf_iterator<char>());
	}
	cl::Program::Sources clSource(1, pair<const char*, int>(clCode.c_str(), clCode.length()+1));
	program = cl::Program(context, clSource);
	if (program.build(devices, "-I.")) {
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << endl;
		exit(1);
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	igl = cl::ImageGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture, NULL);
	bs.push_back(igl);
	cq = cl::CommandQueue(context, devices[0], 0, NULL);
	glDisable(GL_TEXTURE_2D);
}

void HolyGameViewController::render() {
	glDisable(GL_DEPTH_TEST);
	if (world->getMyObject() == NULL) {
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	map<int, MovingRoundObject*> const& roundObjects = world->getMovingRoundObjects();

	float focusx = 0.0f;
	float focusy = 0.0f;
	if (world->getMyObject() != NULL) {
		focusx = world->getMyObject()->center.x;
		focusy = world->getMyObject()->center.y;
	}
	if (focusx < world->getMinX()+14) focusx += (14-focusx+world->getMinX())*(14-focusx+world->getMinX())/28.0;
	if (focusx > world->getMaxX()-14) focusx -= (14+focusx-world->getMaxX())*(14+focusx-world->getMaxX())/28.0;
	if (focusy < world->getMinY()+14) focusy += (14-focusy+world->getMinY())*(14-focusy+world->getMinY())/28.0;
	if (focusy > world->getMaxY()-14) focusy -= (14+focusy-world->getMaxY())*(14+focusy-world->getMaxY())/28.0;

	map<int, RectangularWall*> const& rectangularWalls = world->getRectangularWalls();
	map<int, RoundWall*> const& roundWalls = world->getRoundWalls();
	
	const int numWallsInRound = 360;

	float obspoints[4*rectangularWalls.size() + numWallsInRound*roundWalls.size()*4];
	unsigned char obscolor[4*rectangularWalls.size() + numWallsInRound*roundWalls.size()*4];
	unsigned ti, i2 = 0;
	map<int, RoundWall*>::const_iterator i3 = roundWalls.begin();
	int i3Component = 0;
	for (ti = 0; ; ti++) {
		Vector2D p1, p2;
		MaterialPtr material;
		while (i2 != rectangularWalls.size() || i3 != roundWalls.end()) {
			if (i2 != rectangularWalls.size()) {
				RectangularWall* wall = rectangularWalls.find(i2)->second;
				p1 = wall->p1;
				p2 = wall->p2;
				i2++;
				material = wall->getMaterial();
			} else {
				RoundWall* wall = i3->second;
				float ratio = (float)i3Component / (float)(numWallsInRound+1);
				p1 = wall->center + wall->radius * Vector2D::getUnitVector(wall->theta2 * ratio + wall->theta1 * (1.0 - ratio));
				ratio = (float)(i3Component+1) / (float)(numWallsInRound+1);
				p2 = wall->center + wall->radius * Vector2D::getUnitVector(wall->theta2 * ratio + wall->theta1 * (1.0 - ratio));
				i3Component++;
				if (i3Component == numWallsInRound) {
					i3Component = 0;
					++i3;
				}
				material = wall->getMaterial();
			}
			Vector2D diff = Vector2D(focusx, focusy) - p1;
			Vector2D obsdir = p2 - p1;
			float smallest;
			if (diff*obsdir <= 0) smallest = diff*diff;
			else if (diff*obsdir >= obsdir*obsdir) smallest = (diff-obsdir)*(diff-obsdir);
			else smallest = diff*diff-(diff*obsdir)*(diff*obsdir)/(obsdir*obsdir);
			if (smallest <= 56*56*(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)) break;
		}
		if (i2 == rectangularWalls.size() && i3 == roundWalls.end()) {
			break;
		}
		obspoints[4*ti] = p1.x;
		obspoints[4*ti+1] = p1.y;
		obspoints[4*ti+2] = p2.x;
		obspoints[4*ti+3] = p2.y;
		obscolor[4*ti] = material->getR();
		obscolor[4*ti+1] = material->getG();
		obscolor[4*ti+2] = material->getB();
		obscolor[4*ti+3] = material->getA();
	}

	cl::Buffer obspointsbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*ti*sizeof(float), obspoints, NULL);
	cl::Buffer obscolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*ti*sizeof(char), obscolor, NULL);
	
	float objpoint[2*roundObjects.size()];
	float objsize[2*roundObjects.size()];
	unsigned char objcolor[4*roundObjects.size()];
	auto it = roundObjects.begin();
	int si;
	for (si = 0; ; si++) {
		while (it != roundObjects.end() && (focusx-it->second->center.x)*(focusx-it->second->center.x)+(focusy-it->second->center.y)*(focusx-it->second->center.y) > (56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second->radius)*(56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second->radius)) it++;
		if (it == roundObjects.end()) break;
		objpoint[2*si] = it->second->center.x;
		objpoint[2*si+1] = it->second->center.y;
		objsize[2*si] = it->second->radius;
		objsize[2*si+1] = it->second->heightRatio;
		MaterialPtr color = it->second->getMaterial();
		objcolor[4*si] = color->getR();
		objcolor[4*si+1] = color->getG();
		objcolor[4*si+2] = color->getB();
		objcolor[4*si+3] = color->getA();
		it++;
	}
	cl::Buffer objpointbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*roundObjects.size()*sizeof(float), objpoint, NULL);
	cl::Buffer objsizebuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*roundObjects.size()*sizeof(float), objsize, NULL);
	cl::Buffer objcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*roundObjects.size()*sizeof(char), objcolor, NULL);
	
	float lightpos[3*world->getLights().size()];
	unsigned char lightcolor[4*world->getLights().size()];
	for (unsigned i = 0; i < world->getLights().size(); i++) {
		lightpos[3*i] = world->getLights()[i].getPosition().x;
		lightpos[3*i+1] = world->getLights()[i].getPosition().y;
		lightpos[3*i+2] = world->getLights()[i].getPosition().z;
		lightcolor[4*i] = world->getLights()[i].getColor().getR();
		lightcolor[4*i+1] = world->getLights()[i].getColor().getG();
		lightcolor[4*i+2] = world->getLights()[i].getColor().getB();
		lightcolor[4*i+3] = world->getLights()[i].getColor().getA();
	}
	cl::Buffer lightposbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 3*world->getLights().size()*sizeof(float), lightpos, NULL);
	cl::Buffer lightcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world->getLights().size()*sizeof(char), lightcolor, NULL);
	
	cq.enqueueAcquireGLObjects(&bs);
	cl::Kernel renderKern(program, "render", NULL);
	renderKern.setArg(0, focusx-7*cosf(angle));
	renderKern.setArg(1, focusy-7*sinf(angle));
	renderKern.setArg(2, 4.0f);
	renderKern.setArg(3, cosf(angle));
	renderKern.setArg(4, sinf(angle));
	renderKern.setArg(5, -4.0f/7);
	renderKern.setArg(6, ti);
	renderKern.setArg(7, obspointsbuf);
	renderKern.setArg(8, obscolorbuf);
	renderKern.setArg(9, si);
	renderKern.setArg(10, objpointbuf);
	renderKern.setArg(11, objsizebuf);
	renderKern.setArg(12, objcolorbuf);
	renderKern.setArg(13, (int)world->getLights().size());
	renderKern.setArg(14, lightposbuf);
	renderKern.setArg(15, lightcolorbuf);
	renderKern.setArg(16, igl);
	renderKern.setArg(17, WIDTH);
	renderKern.setArg(18, HEIGHT);
	renderKern.setArg(19, world->getMinX());
	renderKern.setArg(20, world->getMaxX());
	renderKern.setArg(21, world->getMinY());
	renderKern.setArg(22, world->getMaxY());
	cq.enqueueNDRangeKernel(renderKern, cl::NullRange, cl::NDRange((WIDTH+15)/16*16, (HEIGHT+15)/16*16), cl::NDRange(16, 16));
	cq.enqueueReleaseGLObjects(&bs);
	cq.finish();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
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

