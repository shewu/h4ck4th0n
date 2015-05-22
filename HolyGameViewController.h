#ifndef __HOLYGAMEVIEWCONTROLLER_H__
#define __HOLYGAMEVIEWCONTROLLER_H__

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

class HolyGameViewController : public GameViewController {
	private:
		void _initGL();
		cl::Context context;
		cl::Program program;
		vector<cl::Device> devices;
		cl::CommandQueue cq;
		vector<cl::Memory> bs;
		cl::ImageGL igl;
		GLuint texture;

	public:
		HolyGameViewController();
		~HolyGameViewController();

		void process();
		void render();
        bool change_resolution (int width, int height);
};

#endif

