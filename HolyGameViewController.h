#ifndef __HOLYGAMEVIEWCONTROLLER_H__
#define __HOLYGAMEVIEWCONTROLLER_H__

#ifndef __APPLE__
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "CL/cl.hpp"
#else
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <OpenCL/opencl.h>
#endif

#include "GameViewController.h"

class HolyGameViewController : public GameViewController {
	private:
		void _initGL();
#ifndef __APPLE__
		cl::Context context;
		cl::Program program;
		vector<cl::Device> devices;
		cl::CommandQueue cq;
		vector<cl::Memory> bs;
		cl::Image2DGL igl;
#else
        cl_context context;
        cl_program program;
        vector<cl_device_id> devices;
        cl_command_queue cq;
        vector<cl_mem> bs;
        cl_mem igl;
#endif
		GLuint texture;

	public:
		HolyGameViewController();
		~HolyGameViewController();

		void process();
		void render();
};

#endif

