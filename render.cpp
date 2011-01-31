#include "render.h"
#include <GL/glew.h>
#include <iostream>
#include <SDL/SDL.h>
#include "constants.h"
#include "client.h"

using namespace std;

GLUquadric* quad;

void initGL() {
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ((float) WIDTH) / ((float) HEIGHT), 1.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	quad = gluNewQuadric();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.5, 0.01, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	
	for (map<int, Object>::iterator i = world.objects.begin(); i != world.objects.end(); i++) {
		glPushMatrix();
		glTranslatef(i->second.p.x, i->second.p.y, 0);
		glScalef(i->second.rad, i->second.rad, i->second.h);
		glColor3f(i->second.color.r, i->second.color.g, i->second.color.b);
		gluSphere(quad, 1.0, 30, 30);
		glPopMatrix();
	}
	
	glBegin(GL_QUADS);
	for (vector<Obstacle>::iterator i = world.obstacles.begin(); i != world.obstacles.end(); i++) {
		glColor3f(i->color.r, i->color.g, i->color.b);
		glVertex3f(i->p1.x, i->p1.y, 0);
		glVertex3f(i->p2.x, i->p2.y, 0);
		glVertex3f(i->p2.x, i->p2.y, 1);
		glVertex3f(i->p1.x, i->p1.y, 1);
	}
	glEnd();
}
