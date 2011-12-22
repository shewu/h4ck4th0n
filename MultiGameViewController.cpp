#include <iostream>
#include <fstream>
#include <cmath>

#include "MultiGameViewController.h"

MultiGameViewController::MultiGameViewController() : GameViewController() {
	_initGL();
}

MultiGameViewController::~MultiGameViewController() { }

void MultiGameViewController::process() {
	GameViewController::process();
}

void MultiGameViewController::_initGL() {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

	//context = cl::Context(devices, NULL, NULL, NULL, NULL);
	//devices = context.getInfo<CL_CONTEXT_DEVICES>();

	if (devices.size() <= 0) {
		cout << "No OpenCL devices found. Quitting." << endl;
		exit(1);
	}
	cout << "Number of OpenCL devices found: " << devices.size() << endl;
	for (unsigned i = 0; i < devices.size(); i++) {
		cout << "Device " << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << " Vendor ID: " << devices[i].getInfo<CL_DEVICE_VENDOR_ID>() <<  endl;
		vector<cl::Device> temp;
		temp.push_back(devices[i]);
		deviceContexts.push_back(cl::Context(temp, NULL, NULL, NULL, NULL));	
	}

	string clCode;
	{
		ifstream code("multirender.cl");
		clCode = string((std::istreambuf_iterator<char>(code)), std::istreambuf_iterator<char>());
	}

	cl::Program::Sources clSource(1, pair<const char*, int>(clCode.c_str(), clCode.length() + 1));
	for (unsigned i = 0; i < devices.size(); i++) {
		devicePrograms.push_back(cl::Program(deviceContexts[i], clSource));
		if (devicePrograms[i].build(deviceContexts[i].getInfo<CL_CONTEXT_DEVICES>(), "-I.")) {
			cout << devicePrograms[i].getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[i]) << endl;
			exit(1);
		}
	}
	/*
	if (program.build(devices, "-I.")) {
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << endl;
		exit(1);
	}
	*/

	glViewport(0, 0, WIDTH, HEIGHT);
	glOrtho(-1, 1, -1, 1, -1, 1);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(devices.size(), &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	pixels = new int[WIDTH * HEIGHT];
	for (unsigned i = 0; i < devices.size(); i++) {
		int render_offset = (((i+1)*(HEIGHT/16))/devices.size()-(i*(HEIGHT/16))/devices.size())*16;
		images.push_back(cl::Image2D(deviceContexts[i], CL_MEM_WRITE_ONLY, cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), WIDTH, render_offset));
		cqs.push_back(cl::CommandQueue(deviceContexts[i], devices[i], 0, NULL));
	}

	glDisable(GL_TEXTURE_2D);
}

void MultiGameViewController::render() {
	glDisable(GL_DEPTH_TEST);
	if (myId == -1) {
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.minX + 14) focusx += (14 - focusx + world.minX) * (14 - focusx + world.minX) / 28.0;
	if (focusx > world.maxX - 14) focusx -= (14 + focusx - world.maxX) * (14 + focusx - world.maxX) / 28.0;
	if (focusy < world.minY + 14) focusy += (14 - focusy + world.minY) * (14 - focusy + world.minY) / 28.0;
	if (focusy > world.maxY - 14) focusy -= (14 + focusy - world.maxY) * (14 + focusy - world.maxY) / 28.0;

	float obspoints[4 * world.obstacles.size()];
	unsigned char obscolor[4 * world.obstacles.size()];
	unsigned ti, i2 = 0;
	for (ti = 0; ; ti++) {
		while (i2 != world.obstacles.size()) {
			Vector2D diff = Vector2D(focusx, focusy) - world.obstacles[i2].p1;
			Vector2D obsdir = world.obstacles[i2].p2 - world.obstacles[i2].p1;
			float smallest;
			if (diff * obsdir <= 0) smallest = diff * diff;
			else if (diff * obsdir >= obsdir * obsdir) smallest = (diff - obsdir) * (diff - obsdir);
			else smallest = diff * diff-(diff * obsdir)*(diff * obsdir)/(obsdir * obsdir);
			if (smallest <= 56 * 56 * (1 + float(WIDTH) * float(WIDTH) / float(HEIGHT) / float(HEIGHT) / 4.0)) break;
			i2++;
		}
		if (i2 == world.obstacles.size()) break;
		obspoints[4 * ti] = world.obstacles[i2].p1.x;
		obspoints[4 * ti + 1] = world.obstacles[i2].p1.y;
		obspoints[4 * ti + 2] = world.obstacles[i2].p2.x;
		obspoints[4 * ti + 3] = world.obstacles[i2].p2.y;
		obscolor[4 * ti] = world.obstacles[i2].color.r;
		obscolor[4 * ti + 1] = world.obstacles[i2].color.g;
		obscolor[4 * ti + 2] = world.obstacles[i2].color.b;
		obscolor[4 * ti + 3] = world.obstacles[i2].color.a;
		i2++;
	}
	vector<cl::Buffer> obspointsbuf, obscolorbuf, objpointbuf, objsizebuf, objcolorbuf, lightposbuf, lightcolorbuf;
	for (unsigned i = 0; i < devices.size(); i++) {
		obspointsbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(float), obspoints, NULL));
		obscolorbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(char), obscolor, NULL));
	}

	float objpoint[2 * world.objects.size()];
	float objsize[2 * world.objects.size()];
	unsigned char objcolor[4 * world.objects.size()];
	map<int, Object>::iterator it = world.objects.begin();
	int si;
	for (si = 0; ; si++) {
		while (it != world.objects.end() && (focusx-it->second.p.x)*(focusx-it->second.p.x)+(focusy-it->second.p.y)*(focusx-it->second.p.y) > (56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second.rad)*(56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second.rad)) it++;
		if (it == world.objects.end()) break;
		objpoint[2*si] = it->second.p.x;
		objpoint[2*si+1] = it->second.p.y;
		objsize[2*si] = it->second.rad;
		objsize[2*si+1] = it->second.hrat;
		objcolor[4*si] = it->second.color.r;
		objcolor[4*si+1] = it->second.color.g;
		objcolor[4*si+2] = it->second.color.b;
		objcolor[4*si+3] = it->second.color.a;
		it++;
	}
	for(unsigned i = 0; i < devices.size(); i++) {
		objpointbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objpoint, NULL));
		objsizebuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objsize, NULL));
		objcolorbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.objects.size()*sizeof(char), objcolor, NULL));
	}

	float lightpos[3*world.lights.size()];
	unsigned char lightcolor[4*world.lights.size()];
	for (unsigned i = 0; i < world.lights.size(); i++) {
		lightpos[3*i] = world.lights[i].position.x;
		lightpos[3*i+1] = world.lights[i].position.y;
		lightpos[3*i+2] = world.lights[i].position.z;
		lightcolor[4*i] = world.lights[i].color.r;
		lightcolor[4*i+1] = world.lights[i].color.g;
		lightcolor[4*i+2] = world.lights[i].color.b;
		lightcolor[4*i+3] = world.lights[i].color.a;
	}
	for (unsigned i = 0; i < devices.size(); i++) {
		lightposbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 3*world.lights.size()*sizeof(float), lightpos, NULL));
		lightcolorbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.lights.size()*sizeof(char), lightcolor, NULL));
	}

	for (unsigned i = 0; i < devices.size(); i++) {
		cl::Kernel renderKern(devicePrograms[i], "render", NULL);
		renderKern.setArg(0, focusx-7*cos(angle));
		renderKern.setArg(1, focusy-7*sin(angle));
		renderKern.setArg(2, 4.0f);
		renderKern.setArg(3, cos(angle));
		renderKern.setArg(4, sin(angle));
		renderKern.setArg(5, -4.0f/7);
		renderKern.setArg(6, ti);
		renderKern.setArg(7, obspointsbuf[i]);
		renderKern.setArg(8, obscolorbuf[i]);
		renderKern.setArg(9, si);
		renderKern.setArg(10, objpointbuf[i]);
		renderKern.setArg(11, objsizebuf[i]);
		renderKern.setArg(12, objcolorbuf[i]);
		renderKern.setArg(13, (int)world.lights.size());
		renderKern.setArg(14, lightposbuf[i]);
		renderKern.setArg(15, lightcolorbuf[i]);
		renderKern.setArg(16, images[i]);
		renderKern.setArg(17, WIDTH);
		renderKern.setArg(18, HEIGHT);
		renderKern.setArg(19, world.minX);
		renderKern.setArg(20, world.maxX);
		renderKern.setArg(21, world.minY);
		renderKern.setArg(22, world.maxY);
		renderKern.setArg(23, int(((i*(HEIGHT/16))/devices.size())*16));
		int render_offset = (((i+1)*(HEIGHT/16))/devices.size()-(i*(HEIGHT/16))/devices.size())*16;
		cqs[i].enqueueNDRangeKernel(renderKern, cl::NullRange, cl::NDRange(WIDTH, render_offset), cl::NDRange(16, 16));
		cl::size_t<3> offset, sz;
		offset.push_back(0);
		offset.push_back(0);
		offset.push_back(0);
		sz.push_back(WIDTH);
		sz.push_back((((i+1)*(HEIGHT/16))/devices.size()-(i*(HEIGHT/16))/devices.size())*16);
		sz.push_back(1);
		cqs[i].enqueueReadImage(images[i], false, offset, sz, 0, 0, pixels+((i*(HEIGHT/16))/devices.size())*16*WIDTH);
	}
	for (unsigned i = 0; i < devices.size(); i++) cqs[i].finish();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
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

