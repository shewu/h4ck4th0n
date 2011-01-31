#include "render.h"
#include <GL/glew.h>
#include <iostream>
#include <SDL/SDL.h>
#include "constants.h"
#include "client.h"

using namespace std;

void render()
{
	cout << "entering render" << endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ((float) WIDTH) / ((float) HEIGHT), 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(3.0, 3.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	GLUquadric* quad = gluNewQuadric();
	glColor3f(1.0, 1.0, 1.0);
	gluSphere(quad, 1.0, 30, 30);
	SDL_GL_SwapBuffers();
}
