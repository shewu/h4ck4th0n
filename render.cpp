#include "render.h"
#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <CL/cl.hpp>
#include <IL/il.h>
#include <cmath>
#include "client.h"

using namespace std;

GLUquadric* quad;
cl::Context context;
cl::Image2DGL igl;
cl::Image2DGL igl2;
cl::Program program;
vector<cl::Memory> bs;
vector<cl::Device> devices;
cl::CommandQueue cq;

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
		exit(1);
	}
	
	glViewport(0, 0, WIDTH, HEIGHT);
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	glEnable(GL_TEXTURE_2D);
	GLuint texture[2];
	glGenTextures(2, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	igl = cl::Image2DGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture[0], NULL);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	unsigned int image;
	ilInit();
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage("grass.png");
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
	igl2 = cl::Image2DGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, texture[1], NULL);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	bs.push_back(igl);
	bs.push_back(igl2);
	cq = cl::CommandQueue(context, devices[0], 0, NULL);
}

void render()
{
	float obspoints[4*world.obstacles.size()];
	unsigned char obscolor[4*world.obstacles.size()];
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
	
	float objpoint[2*world.objects.size()];
	float objsize[2*world.objects.size()];
	unsigned char objcolor[4*world.objects.size()];
	map<int, Object>::iterator it = world.objects.begin();
	for (int i = 0; i < world.objects.size(); i++) {
		objpoint[2*i] = it->second.p.x;
		objpoint[2*i+1] = it->second.p.y;
		objsize[2*i] = it->second.rad;
		objsize[2*i+1] = it->second.hrat;
		objcolor[4*i] = it->second.color.r;
		objcolor[4*i+1] = it->second.color.g;
		objcolor[4*i+2] = it->second.color.b;
		objcolor[4*i+3] = it->second.color.a;
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
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < MIN_X+14) focusx += (14-focusx+MIN_X)*(14-focusx+MIN_X)/28.0;
	if (focusx > MAX_X-14) focusx -= (14+focusx-MAX_X)*(14+focusx-MAX_X)/28.0;
	if (focusy < MIN_Y+14) focusy += (14-focusy+MIN_Y)*(14-focusy+MIN_Y)/28.0;
	if (focusy > MAX_Y-14) focusy -= (14+focusy-MAX_Y)*(14+focusy-MAX_Y)/28.0;
	
	cq.enqueueAcquireGLObjects(&bs);
	cl::Kernel renderKern(program, "render", NULL);
	renderKern.setArg(0, focusx-7*cos(angle));
	renderKern.setArg(1, focusy-7*sin(angle));
	renderKern.setArg(2, 4.0f);
	renderKern.setArg(3, cos(angle));
	renderKern.setArg(4, sin(angle));
	renderKern.setArg(5, -4.0f/7);
	renderKern.setArg(6, (int)world.obstacles.size());
	renderKern.setArg(7, obspointsbuf);
	renderKern.setArg(8, obscolorbuf);
	renderKern.setArg(9, (int)world.objects.size());
	renderKern.setArg(10, objpointbuf);
	renderKern.setArg(11, objsizebuf);
	renderKern.setArg(12, objcolorbuf);
	renderKern.setArg(13, (int)world.lights.size());
	renderKern.setArg(14, lightposbuf);
	renderKern.setArg(15, lightcolorbuf);
	renderKern.setArg(16, igl);
	renderKern.setArg(17, igl2);
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
