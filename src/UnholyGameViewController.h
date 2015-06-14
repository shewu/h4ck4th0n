#ifndef __UNHOLYGAMEVIEWCONTROLLER_H__
#define __UNHOLYGAMEVIEWCONTROLLER_H__

#ifndef __APPLE__
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#endif

#include "GameViewController.h"

extern bool NORAPE;

class UnholyGameViewController : public GameViewController {
	public:
		UnholyGameViewController();
		~UnholyGameViewController();

		void process();
		void render();

	private:
		GLUquadric* _quad;
		void _initGL();
		void _drawWalls();
		void _drawObjects();
		void _drawFloor(const float);
};

#endif
