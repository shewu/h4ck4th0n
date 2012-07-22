#ifndef MULTIGAMEVIEWCONTROLLER_H
#define MULTIGAMEVIEWCONTROLLER_H

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
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

