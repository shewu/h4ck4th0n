#include "render.h"
#include <GL/glew.h>

void render()
{
	GLUquadric* quad = gluNewQuadric();
	glColor3f(1.0, 1.0, 1.0);
	gluSphere(quad, 0.1, 10, 10);
}
