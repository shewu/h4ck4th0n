#include "render.h"
#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenCL/opencl.h>
#else
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "cl.hpp"
#include <cmath>
#include "client.h"

using namespace std;

GLUquadric* quad;
cl::Context context;
cl::Image2DGL igl;
cl::Program program;
vector<cl::Memory> bs;
vector<cl::Device> devices;
cl::CommandQueue cq;
GLuint texture;

void initGL() {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
#ifdef __APPLE__
	/* based on the Apple OpenCL documentation */
	// declare memory to hold the device id
	cl_device_id device_id;
	
	// ask OpenCL for exactly 1 GPU
	cl_int err = clGetDeviceIDs(
								NULL,				// platform ID
								CL_DEVICE_TYPE_GPU,	// look only for GPUs
								1,					// return an ID for only one GPU
								&device_id,			// on return, the device ID
								NULL);				// don't return the number of devices
	
	// make sure nothing went wrong
	if (err != CL_SUCCESS) {
		cout << "Something, somewhere went terribly wrong.\n";
		exit(1);
	}
	
	context = clCreateContext(
							  0, 
							  numDevices,	// the number of devices in the devices parameter
							  &devices,		// a pointer to the list of device IDs from clGetDeviceIDs
							  NULL,			// a pointer to an error notice callback function (if any)
							  NULL,			// data to pass as a param to the callback function
							  &err);		// on return, points to a result code
	
	// again, make sure nothing went wrong
	if (err != CL_SUCCESS) {
		cout << "Something, somewhere went terribly wrong.\n";
		exit(1);
	}
#else
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
#endif
	if (devices.size() <=0) {
		cout << "No OpenCL devices found. Quitting." << endl;
		exit(1);
	}
	string clCode;
	{
		ifstream code("render.cl");
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

void render()
{
	glDisable(GL_DEPTH_TEST);
	if (myId == -1) {
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.minX+14) focusx += (14-focusx+world.minX)*(14-focusx+world.minX)/28.0;
	if (focusx > world.maxX-14) focusx -= (14+focusx-world.maxX)*(14+focusx-world.maxX)/28.0;
	if (focusy < world.minY+14) focusy += (14-focusy+world.minY)*(14-focusy+world.minY)/28.0;
	if (focusy > world.maxY-14) focusy -= (14+focusy-world.maxY)*(14+focusy-world.maxY)/28.0;
	
	float obspoints[4*world.obstacles.size()];
	unsigned char obscolor[4*world.obstacles.size()];
	int ti, i2 = 0;
	for (ti = 0; ; ti++) {
		while (i2 != world.obstacles.size()) {
			Vector2D diff = Vector2D(focusx, focusy)-world.obstacles[i2].p1;
			Vector2D obsdir = world.obstacles[i2].p2-world.obstacles[i2].p1;
			float smallest;
			if (diff*obsdir <= 0) smallest = diff*diff;
			else if (diff*obsdir >= obsdir*obsdir) smallest = (diff-obsdir)*(diff-obsdir);
			else smallest = diff*diff-(diff*obsdir)*(diff*obsdir)/(obsdir*obsdir);
			if (smallest <= 56*56*(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)) break;
			i2++;
		}
		if (i2 == world.obstacles.size()) break;
		obspoints[4*ti] = world.obstacles[i2].p1.x;
		obspoints[4*ti+1] = world.obstacles[i2].p1.y;
		obspoints[4*ti+2] = world.obstacles[i2].p2.x;
		obspoints[4*ti+3] = world.obstacles[i2].p2.y;
		obscolor[4*ti] = world.obstacles[i2].color.r;
		obscolor[4*ti+1] = world.obstacles[i2].color.g;
		obscolor[4*ti+2] = world.obstacles[i2].color.b;
		obscolor[4*ti+3] = world.obstacles[i2].color.a;
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
		objcolor[4*si] = it->second.color.r;
		objcolor[4*si+1] = it->second.color.g;
		objcolor[4*si+2] = it->second.color.b;
		objcolor[4*si+3] = it->second.color.a;
		it++;
	}
	cl::Buffer objpointbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objpoint, NULL);
	cl::Buffer objsizebuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objsize, NULL);
	cl::Buffer objcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.objects.size()*sizeof(char), objcolor, NULL);
	
	float lightpos[3*world.lights.size()];
	unsigned char lightcolor[4*world.lights.size()];
	for (int i = 0; i < world.lights.size(); i++) {
		lightpos[3*i] = world.lights[i].position.x;
		lightpos[3*i+1] = world.lights[i].position.y;
		lightpos[3*i+2] = world.lights[i].position.z;
		lightcolor[4*i] = world.lights[i].color.r;
		lightcolor[4*i+1] = world.lights[i].color.g;
		lightcolor[4*i+2] = world.lights[i].color.b;
		lightcolor[4*i+3] = world.lights[i].color.a;
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
	renderKern.setArg(19, world.minX);
	renderKern.setArg(20, world.maxX);
	renderKern.setArg(21, world.minY);
	renderKern.setArg(22, world.maxY);
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
}
