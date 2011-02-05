#include "render.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#include <GL/glx.h>
#include <CL/cl.hpp>
#include <IL/il.h>
#include <cmath>
#include "constants.h"
#include "client.h"

using namespace std;

GLUquadric* quad;
cl::Context context;
cl::Image2DGL igl;
cl::Program program;
vector<cl::Memory> bs;
vector<cl::Device> devices;

void initGL() {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	cl_context_properties props[7] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), CL_GLX_DISPLAY_KHR, (intptr_t)glXGetCurrentDisplay(), CL_GL_CONTEXT_KHR, (intptr_t)glXGetCurrentContext(), 0 };
	context = cl::Context(CL_DEVICE_TYPE_GPU, props, NULL, NULL, NULL);
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	
	string clCode;
	{
		ifstream code("render.cl");
		clCode = string((std::istreambuf_iterator<char>(code)), std::istreambuf_iterator<char>());
	}
	
	cl::Program::Sources clSource(1, pair<const char*, int>(clCode.c_str(), clCode.length()+1));
	program = cl::Program(context, clSource);
	if (program.build(devices, "-I.")) {
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << endl;
		_exit(1);
	}
	
	glViewport(0, 0, WIDTH, HEIGHT);
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	glEnable(GL_TEXTURE_2D);
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	igl = cl::Image2DGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture, NULL);
	bs.push_back(igl);
}

void render()
{
	float obspoints[4*world.obstacles.size()];
	char obscolor[4*world.obstacles.size()];
	for (int i = 0; i < world.obstacles.size(); i++) {
		obspoints[4*i] = world.obstacles[i].p1.x;
		obspoints[4*i+1] = world.obstacles[i].p1.y;
		obspoints[4*i+2] = world.obstacles[i].p2.x;
		obspoints[4*i+3] = world.obstacles[i].p2.y;
		obscolor[4*i] = world.obstacles[i].color.r;
		obscolor[4*i+1] = world.obstacles[i].color.g;
		obscolor[4*i+2] = world.obstacles[i].color.b;
		obscolor[4*i+3] = world.obstacles[i].color.a;
	}
	cl::Buffer obspointsbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(float), obspoints, NULL);
	cl::Buffer obscolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(char), obscolor, NULL);
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < MIN_X+6) focusx = MIN_X+6;
	if (focusx > MAX_X-6) focusx = MAX_X-6;
	if (focusy < MIN_Y+6) focusy = MIN_Y+6;
	if (focusy > MAX_Y-6) focusy = MAX_Y-6;
	
	cl::CommandQueue cq(context, devices[0], 0, NULL);
	cq.enqueueAcquireGLObjects(&bs);
	cl::Kernel renderKern(program, "render", NULL);
	renderKern.setArg(0, (int)world.obstacles.size());
	renderKern.setArg(1, focusx-6*cos(angle));
	renderKern.setArg(2, focusy-6*sin(angle));
	renderKern.setArg(3, 3);
	renderKern.setArg(4, cos(angle));
	renderKern.setArg(5, sin(angle));
	renderKern.setArg(6, -1/2);
	renderKern.setArg(7, obspointsbuf);
	renderKern.setArg(8, obscolorbuf);
	renderKern.setArg(9, igl);
	cq.enqueueNDRangeKernel(renderKern, cl::NullRange, cl::NDRange((WIDTH+15)/16*16, (HEIGHT+15)/16*16), cl::NDRange(16, 16));
	cq.enqueueReleaseGLObjects(&bs);
	cq.finish();
	
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
	glEnd();
}
