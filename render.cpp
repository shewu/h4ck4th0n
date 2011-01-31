#include "render.h"
#include <GL/glew.h>

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(3.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	GLUquadric* quad = gluNewQuadric();
	glColor3f(1.0, 1.0, 1.0);
	gluSphere(quad, 1.0, 10, 10);
}
