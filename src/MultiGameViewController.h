#ifndef MULTIGAMEVIEWCONTROLLER_H
#define MULTIGAMEVIEWCONTROLLER_H

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
		std::vector<cl::Context> deviceContexts;
		std::vector<cl::Program> devicePrograms;
		std::vector<cl::Device> devices;
		std::vector<cl::Image2D> images;
		std::vector<cl::CommandQueue> cqs;
		GLuint texture;
		int* pixels;

	public:
		MultiGameViewController();
		~MultiGameViewController();

		void process();
		void render();
};

#endif

