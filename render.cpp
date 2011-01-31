#include "render.h"
#include <GL/glew.h>
#include <iostream>
#include <SDL/SDL.h>
#include <IL/il.h>
#include <cmath>
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
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	
	unsigned int texture, image;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	ilInit();
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage("grass.png");
	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), GL_RGB, GL_UNSIGNED_BYTE, ilGetData());
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(world.objects[myId].p.x-8*cos(angle), world.objects[myId].p.y-8*sin(angle), 3, world.objects[myId].p.x, world.objects[myId].p.y, 0.0, 0.0, 0.0, 1.0);
	
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
	
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(MIN_X, MIN_Y, 0);
	glTexCoord2f(10.0f, 0.0f);
	glVertex3f(MAX_X, MIN_Y, 0);
	glTexCoord2f(10.0f, 10.0f);
	glVertex3f(MAX_X, MAX_Y, 0);
	glTexCoord2f(0.0f, 10.0f);
	glVertex3f(MIN_X, MAX_Y, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
