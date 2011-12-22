#include <cmath>

#include "UnholyGameViewController.h"

UnholyGameViewController::UnholyGameViewController() : GameViewController() {
	_initGL();
}

UnholyGameViewController::~UnholyGameViewController() { }

void UnholyGameViewController::process() {
	GameViewController::process();
}

void UnholyGameViewController::_initGL() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ((float) WIDTH) / ((float) HEIGHT), 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	quad = gluNewQuadric();
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	GLfloat light_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat light_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[] = { 0.8, 0.8, 0.8, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

	glEnable(GL_LIGHTING); // enable lighting 
	glEnable(GL_LIGHT0); // enable light 0

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void UnholyGameViewController::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE_ARB);

	glLoadIdentity();
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.minX+6) focusx = world.minX+6;
	if (focusx > world.maxX-6) focusx = world.maxX-6;
	if (focusy < world.minY+6) focusy = world.minY+6;
	if (focusy > world.maxY-6) focusy = world.maxY-6;
	
	gluLookAt(focusx-6*cos(angle), focusy-6*sin(angle), 3, focusx, focusy, 0.0, 0.0, 0.0, 1.0);
	float matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	
	// fake reflections
	glPushMatrix();
		glScalef(1, 1, -1);
		_drawObjects();
		_drawWalls();
	glPopMatrix();

	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		_drawFloor(.82);
	glDisable(GL_BLEND);

	// draw the things right-side up
	_drawObjects();
	_drawWalls();

	glDisable(GL_MULTISAMPLE_ARB);

	glFlush();

	if (mainmenu->is_active()) {
		glDisable(GL_LIGHTING);
		mainmenu->drawMenu();
		glEnable(GL_LIGHTING);
	}
}

void UnholyGameViewController::_drawWalls() {
	// obstaclesUnholyGameViewController* gvc;
	//glUseProgram(0);
	glBegin(GL_QUADS);
	for (vector<Obstacle>::iterator i = world.obstacles.begin(); i != world.obstacles.end(); i++) {
		glColor3f(i->color.r/255.0, i->color.g/255.0, i->color.b/255.0);
		glVertex3f(i->p1.x, i->p1.y, 0);
		glVertex3f(i->p2.x, i->p2.y, 0);
		glVertex3f(i->p2.x, i->p2.y, 1);
		glVertex3f(i->p1.x, i->p1.y, 1);
	}
	glEnd();
}

void UnholyGameViewController::_drawObjects() {
	glEnable(GL_NORMALIZE);
	for (map<int, Object>::iterator i = world.objects.begin(); i != world.objects.end(); i++) {
		glPushMatrix();
		glTranslatef(i->second.p.x, i->second.p.y, 0);
		glScalef(i->second.rad, i->second.rad, i->second.hrat*i->second.rad);
		glColor3f(i->second.color.r/255.0, i->second.color.g/255.0, i->second.color.b/255.0);
		gluSphere(quad, 1.0, 50, 50);
		glPopMatrix();
	}
	glDisable(GL_NORMALIZE);
}

void UnholyGameViewController::_drawFloor(float alpha) {
	// checkerboard
	unsigned int GridSizeX = world.maxX/3;
	unsigned int GridSizeY = world.maxY/3;
	unsigned int SizeX = 6;
	unsigned int SizeY = 6;

	glEnable(GL_NORMALIZE);
	glBegin(GL_QUADS);
	for (unsigned x = 0; x < GridSizeX; ++x) {
		for (unsigned y = 0; y < GridSizeY; ++y) {
			if ((x+y) % 2 == 1) {//modulo 2
				glColor4f(1.0f,1.0f,1.0f, alpha); //white
			} else {
				glColor4f(0.0f,0.0f,0.0f, alpha); //black
			}

			glNormal3f(                  0,                  0, 1);
			glVertex3f(    x*SizeX + world.minX,    y*SizeY + world.minY, 0);
			glVertex3f((x+1)*SizeX + world.minX,    y*SizeY + world.minY, 0);
			glVertex3f((x+1)*SizeX + world.minX,(y+1)*SizeY + world.minY, 0);
			glVertex3f(    x*SizeX + world.minX,(y+1)*SizeY + world.minY, 0);

		}
	}
	glEnd();
	glDisable(GL_NORMALIZE);
}

