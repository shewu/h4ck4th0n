#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include <fstream>
#include <SDL/SDL.h>

#include "GameViewController.h"

static HBViewMode finishedView = kHBNoView;

static void send_disconnect_message() {
    char q = 0;
    sc_static->add(&q, 1);
    sc_static->send();
}

class quitfunc {
	public:
	quitfunc() {}
	quitfunc(GameViewController* gvc) {
		_gvc = gvc;
	}
	GameViewController* _gvc;
	bool operator()(voidtype) {
		return _gvc->quit();
	}
};

class leavefunc {
	public:
	leavefunc() {}
	leavefunc(GameViewController* gvc) {
		_gvc = gvc;
	}
	GameViewController* _gvc;
	bool operator()(voidtype) {
		return _gvc->leave();
	}
};

static voidtype action_toggle_fullscreen(bool b) {
	if(b) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	}
}

static bool validator_test(char *a) {
	return a[0] == 'a';
}

void GameViewController::_initMenus() {
	mainmenu = new menu();
	mainmenu->add_menuitem(new actionmenuitem(leavefunc(this), (char *)"Leave Game"));
#ifndef __APPLE__
	mainmenu->add_menuitem(new togglemenuitem((char*)"Fullscreen", false, action_toggle_fullscreen));
#endif
	mainmenu->add_menuitem(new actionmenuitem(quitfunc(this), (char *)"Quit"));
}

void GameViewController::_initSound() {
#ifndef __APPLE__
	ALCdevice* dev;
	ALCcontext* con;
	
	dev = alcOpenDevice(NULL);
	con = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(con);
	
	ALfloat pos[] = { 0, 0, 0 };
	ALfloat vel[] = { 0, 0, 0 };
	ALfloat ori[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
	
	alutInit(NULL, NULL);
	albuf[0] = alutCreateBufferFromFile("sounds/boing2.wav");
	albuf[1] = alutCreateBufferFromFile("sounds/splat2.wav");
	albuf[2] = alutCreateBufferFromFile("sounds/ding.wav");
	alGenSources(ALSRCS, alsrcs);
#endif
}

GameViewController::GameViewController() {
	finishedView = kHBNoView;
	_initGL();
	_initSound();
	_initMenus();
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	cout << "Starting client\n";
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	getaddrinfo(ipaddy, "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	Socket* sock = new Socket(sockfd);
	sc = sock->connect(*res->ai_addr, res->ai_addrlen);
	
	int u[2];
	bool done = false;
	for (int i = 0; i < 10 && !done; i++) {
		WritePacket* wp = new WritePacket(CTS_CONNECT, 0); 
        sc->send(wp);
        delete wp;

		SDL_Delay(200);
		ReadPacket* rp;
		while ((rp = sc->receive((char*)&u, 8)) != NULL) {
			if (rp->message_type == STC_INITIAL_ANGLE) {
				done = true;
				break;
			}
            delete rp;
		}
	}
	if (!done) {
		cout << "Failed to connect" << endl;
		leave();
	}

	angle = rp->read_float();
    delete rp;

    myId = -1;
}

GameViewController::~GameViewController() {
	finishedView = kHBNoView;
	SDL_ShowCursor(true);
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	delete sc;
	delete mainmenu;
}

HBViewMode GameViewController::didFinishView() {
	return finishedView;
}

void GameViewController::process() {
	int status;
	
	int count = 0, oldTime = SDL_GetTicks();
	// seems like this is crashing on the second time it's called
	while ((status = world.receiveObjects(sc, myId)) != -1) {
		int time = SDL_GetTicks();
		if (((++count)%=100) == 0) {
			int time = SDL_GetTicks();
			float fps = 100000./(time - oldTime);
			printf("\r");
			cout.width(6);
			cout << (int)fps << "fps" << flush;
			oldTime = time;
		}

		if (status == 0) continue;
#ifndef __APPLE__
		for(vector<pair<char, Vector2D> >::iterator it = world.sounds.begin(); it != world.sounds.end(); it++) {
			int src = -1;
			for (int s = 0; s < ALSRCS; s++) {
				int st;
				alGetSourcei(alsrcs[s], AL_SOURCE_STATE, &st);
				if (st != AL_PLAYING) {
					src = s;
					break;
				}
			}
			if (src >= 0 && src < 3) {
				ALfloat alsrcpos[] = { it->second.x, it->second.y, 0 };
				ALfloat alsrcvel[] = { 0, 0, 0 };

				alSourcef(alsrcs[src], AL_PITCH, 1.0f);
				alSourcef(alsrcs[src], AL_GAIN, 1.0f);
				alSourcefv(alsrcs[src], AL_POSITION, alsrcpos);
				alSourcefv(alsrcs[src], AL_VELOCITY, alsrcvel);
				alSourcei(alsrcs[src], AL_BUFFER, albuf[it->first]);
				alSourcei(alsrcs[src], AL_LOOPING, AL_FALSE);
				alSourcePlay(alsrcs[src]);
			}
		}
#endif
	}
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					if (mainmenu->is_active()) {
						SDL_ShowCursor(false);
						SDL_WM_GrabInput(SDL_GRAB_ON);
						mainmenu->set_active(false);
					} else {
						SDL_ShowCursor(true);
						SDL_WM_GrabInput(SDL_GRAB_OFF);
						mainmenu->set_active(true);
					}
				}
				mainmenu->key_input(event.key.keysym.sym);
				break;
			case SDL_QUIT:
				quit();
				break;
			case SDL_MOUSEMOTION:
				if (mainmenu->is_active()) break;
				angle -= event.motion.xrel/400.0;

				while (angle >= 2*M_PI) angle -= 2*M_PI;
				while (angle < 0) angle += 2*M_PI;
				break;
		}
	}
	
	Uint8* keystate = SDL_GetKeyState(NULL);
	char buf[5];
	*((int*)buf) = htonl(*reinterpret_cast<int*>(&angle));
	buf[4] = 0;
	if(!mainmenu->is_active()) {
		if (keystate[SDLK_a]) buf[4] ^= 1;
		if (keystate[SDLK_d]) buf[4] ^= 2;
		if (keystate[SDLK_w]) buf[4] ^= 4;
		if (keystate[SDLK_s]) buf[4] ^= 8;
	}
	sc->add(buf, 5);
	sc->send();
	
#ifndef __APPLE__
	ALfloat alpos[] = { world.objects[myId].p.x, world.objects[myId].p.y, 0 };
	ALfloat alvel[] = { world.objects[myId].v.x, world.objects[myId].v.y, 0 };
	ALfloat alori[] = { 0.0, cos(angle), sin(angle), 0.0, 1.0, 0.0 };
	alListenerfv(AL_POSITION, alpos);
	alListenerfv(AL_VELOCITY, alvel);
	alListenerfv(AL_ORIENTATION, alori);
#endif
}

void GameViewController::_initGL() {
#ifndef __APPLE__
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		cout << "glewInit failed; " << glewGetErrorString(err) << "\n";
		exit(-1);
	}
#endif

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
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 200.0);

	glEnable(GL_LIGHTING); // enable lighting 
	glEnable(GL_LIGHT0); // enable light 0

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void GameViewController::render() {
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
	

	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightv"), 5*matrix[8]+matrix[12], 5*matrix[9]+matrix[13], 5*matrix[10]+matrix[14]);

	glPushMatrix();
		glScalef(1, 1, -1);
		_drawObjects();
		_drawWalls();
	glPopMatrix();

	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		_drawFloor(0.82);
	glDisable(GL_BLEND);

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

void GameViewController::_drawWalls() {
	// obstaclesGameViewController* gvc;
	glUseProgram(0);
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

void GameViewController::_drawObjects() {
	glEnable(GL_NORMALIZE);
	int howMany = 0;
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

void GameViewController::_drawFloor(float alpha) {
	// checkerboard
	unsigned int GridSizeX = world.maxX/3;
	unsigned int GridSizeY = world.maxY/3;
	unsigned int SizeX = 6;
	unsigned int SizeY = 6;

	glEnable(GL_NORMALIZE);
	glBegin(GL_QUADS);
	for (int x = 0; x < GridSizeX; ++x)
	{
		for (int y = 0; y < GridSizeY; ++y)
		{
			if (abs(x+y) & 1) //modulo 2
				glColor4f(1.0f,1.0f,1.0f, alpha); //white
			else
				glColor4f(0.0f,0.0f,0.0f, alpha); //black

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

void GameViewController::_disconnect() {
	if(sc)
	{
		char q = 0;
		sc->add(&q, 1);
		sc->send();
	}
}

bool GameViewController::quit() {
	_disconnect();
	exit(0);
}

bool GameViewController::leave() {
	_disconnect();
	return finishedView = kHBServerConnectView;
}
