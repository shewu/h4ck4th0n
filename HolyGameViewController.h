#ifndef HOLYGAMEVIEWCONTROLLER_H
#define HOLYGAMEVIEWCONTROLLER_H

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "CL/cl.hpp"

#include "GameViewController.h"

class HolyGameViewController : public GameViewController {
	private:
		void _initGL();
		cl::Context context;
		cl::Program program;
		std::vector<cl::Device> devices;
		cl::CommandQueue cq;
		std::vector<cl::Memory> bs;
		cl::Image2DGL igl;
		GLuint texture;

	public:
		HolyGameViewController();
		~HolyGameViewController();

		void process();
		void render();
};

#endif

