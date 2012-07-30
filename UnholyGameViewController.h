#ifndef __UNHOLYGAMEVIEWCONTROLLER_H__
#define __UNHOLYGAMEVIEWCONTROLLER_H__

#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "GameViewController.h"

extern bool NORAPE;

class UnholyGameViewController : public GameViewController {
	private:
		GLUquadric* _quad;
		void _initGL();
		void _drawWalls();
		void _drawObjects();
		void _drawFloor(float);

	public:
		UnholyGameViewController();
		~UnholyGameViewController();

		void process();
		void render();
};

#endif
