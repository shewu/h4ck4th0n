#ifndef HOLYGAMEVIEWCONTROLLER_H
#define HOLYGAMEVIEWCONTROLLER_H

#include "CL/cl2.hpp"
#ifndef __APPLE__
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#endif

#include "GameViewController.h"

class HolyGameViewController : public GameViewController {
private:
    void _initGL();
    cl::Context context;
    cl::Program program;
    std::vector<cl::Device> devices;
    cl::CommandQueue cq;
    std::vector<cl::Memory> bs;
    cl::ImageGL igl;
    GLuint texture;

public:
    HolyGameViewController();
    ~HolyGameViewController();

    void process();
    void render();
    bool change_resolution(int width, int height);
};

#endif
