#ifndef __MULTIGAMEVIEWCONTROLLER_H__
#define __MULTIGAMEVIEWCONTROLLER_H__

#ifndef __APPLE__
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#endif
#include "CL/cl.hpp"

#include "GameViewController.h"

class MultiGameViewController : public GameViewController {
	private:
		void _initGL();
		vector<cl::Context> deviceContexts;
		vector<cl::Program> devicePrograms;
		vector<cl::Device> devices;
		vector<cl::Image2D> images;
		vector<cl::CommandQueue> cqs;
		GLuint texture;
		int* pixels;

	public:
		MultiGameViewController();
		~MultiGameViewController();

		void process();
		void render();
};

#endif

