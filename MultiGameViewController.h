#ifndef MULTIGAMEVIEWCONTROLLER_H
#define MULTIGAMEVIEWCONTROLLER_H

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <CL/cl.hpp>

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

