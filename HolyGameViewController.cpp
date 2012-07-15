#include "HolyGameViewController.h"

#include <iostream>
#include <fstream>
#include <cmath>

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
	cl_context_properties props[7] = {
		CL_CONTEXT_PLATFORM, 
		(cl_context_properties)(platforms[0])(), 
		CL_GLX_DISPLAY_KHR, 
		(intptr_t)glXGetCurrentDisplay(), 
		CL_GL_CONTEXT_KHR, 
		(intptr_t)glXGetCurrentContext(), 
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
	igl = cl::Image2DGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture, NULL);
	bs.push_back(igl);
	cq = cl::CommandQueue(context, devices[0], 0, NULL);
	glDisable(GL_TEXTURE_2D);
}

void HolyGameViewController::render() {
	glDisable(GL_DEPTH_TEST);
	if (myId == -1) {
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.getMinX()+14) focusx += (14-focusx+world.getMinX())*(14-focusx+world.getMinX())/28.0;
	if (focusx > world.getMaxX()-14) focusx -= (14+focusx-world.getMaxX())*(14+focusx-world.getMaxX())/28.0;
	if (focusy < world.getMinY()+14) focusy += (14-focusy+world.getMinY())*(14-focusy+world.getMinY())/28.0;
	if (focusy > world.getMaxY()-14) focusy -= (14+focusy-world.getMaxY())*(14+focusy-world.getMaxY())/28.0;
	
	float obspoints[4*world.obstacles.size()];
	unsigned char obscolor[4*world.obstacles.size()];
	int ti, i2 = 0;
	for (ti = 0; ; ti++) {
		while ((unsigned)i2 != world.obstacles.size()) {
			Vector2D diff = Vector2D(focusx, focusy)-world.obstacles[i2].p1;
			Vector2D obsdir = world.obstacles[i2].p2-world.obstacles[i2].p1;
			float smallest;
			if (diff*obsdir <= 0) smallest = diff*diff;
			else if (diff*obsdir >= obsdir*obsdir) smallest = (diff-obsdir)*(diff-obsdir);
			else smallest = diff*diff-(diff*obsdir)*(diff*obsdir)/(obsdir*obsdir);
			if (smallest <= 56*56*(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)) break;
			i2++;
		}
		if ((unsigned)i2 == world.obstacles.size()) break;
		obspoints[4*ti] = world.obstacles[i2].p1.x;
		obspoints[4*ti+1] = world.obstacles[i2].p1.y;
		obspoints[4*ti+2] = world.obstacles[i2].p2.x;
		obspoints[4*ti+3] = world.obstacles[i2].p2.y;
		obscolor[4*ti] = world.obstacles[i2].color.getR();
		obscolor[4*ti+1] = world.obstacles[i2].color.getG();
		obscolor[4*ti+2] = world.obstacles[i2].color.getB();
		obscolor[4*ti+3] = world.obstacles[i2].color.getA();
		i2++;
	}
	cl::Buffer obspointsbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(float), obspoints, NULL);
	cl::Buffer obscolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(char), obscolor, NULL);
	
	float objpoint[2*world.objects.size()];
	float objsize[2*world.objects.size()];
	unsigned char objcolor[4*world.objects.size()];
	map<int, Object>::iterator it = world.objects.begin();
	int si;
	for (si = 0; ; si++) {
		while (it != world.objects.end() && (focusx-it->second.p.x)*(focusx-it->second.p.x)+(focusy-it->second.p.y)*(focusx-it->second.p.y) > (56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second.rad)*(56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second.rad)) it++;
		if (it == world.objects.end()) break;
		objpoint[2*si] = it->second.p.x;
		objpoint[2*si+1] = it->second.p.y;
		objsize[2*si] = it->second.rad;
		objsize[2*si+1] = it->second.hrat;
		objcolor[4*si] = it->second.color.getR();
		objcolor[4*si+1] = it->second.color.getG();
		objcolor[4*si+2] = it->second.color.getB();
		objcolor[4*si+3] = it->second.color.getA();
		it++;
	}
	cl::Buffer objpointbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objpoint, NULL);
	cl::Buffer objsizebuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objsize, NULL);
	cl::Buffer objcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.objects.size()*sizeof(char), objcolor, NULL);
	
	float lightpos[3*world.lights.size()];
	unsigned char lightcolor[4*world.lights.size()];
	for (unsigned i = 0; i < world.lights.size(); i++) {
		lightpos[3*i] = world.lights[i].getPosition().x;
		lightpos[3*i+1] = world.lights[i].getPosition().y;
		lightpos[3*i+2] = world.lights[i].getPosition().z;
		lightcolor[4*i] = world.lights[i].getColor().getR();
		lightcolor[4*i+1] = world.lights[i].getColor().getG();
		lightcolor[4*i+2] = world.lights[i].getColor().getB();
		lightcolor[4*i+3] = world.lights[i].getColor().getA();
	}
	cl::Buffer lightposbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 3*world.lights.size()*sizeof(float), lightpos, NULL);
	cl::Buffer lightcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.lights.size()*sizeof(char), lightcolor, NULL);
	
	cq.enqueueAcquireGLObjects(&bs);
	cl::Kernel renderKern(program, "render", NULL);
	renderKern.setArg(0, focusx-7*cos(angle));
	renderKern.setArg(1, focusy-7*sin(angle));
	renderKern.setArg(2, 4.0f);
	renderKern.setArg(3, cos(angle));
	renderKern.setArg(4, sin(angle));
	renderKern.setArg(5, -4.0f/7);
	renderKern.setArg(6, ti);
	renderKern.setArg(7, obspointsbuf);
	renderKern.setArg(8, obscolorbuf);
	renderKern.setArg(9, si);
	renderKern.setArg(10, objpointbuf);
	renderKern.setArg(11, objsizebuf);
	renderKern.setArg(12, objcolorbuf);
	renderKern.setArg(13, (int)world.lights.size());
	renderKern.setArg(14, lightposbuf);
	renderKern.setArg(15, lightcolorbuf);
	renderKern.setArg(16, igl);
	renderKern.setArg(17, WIDTH);
	renderKern.setArg(18, HEIGHT);
	renderKern.setArg(19, world.getMinX());
	renderKern.setArg(20, world.getMaxX());
	renderKern.setArg(21, world.getMinY());
	renderKern.setArg(22, world.getMaxY());
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

	if (mainmenu->is_active()) {
		mainmenu->drawMenu();
	}
}

