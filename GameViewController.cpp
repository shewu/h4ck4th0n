#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
#endif

#include "GameViewController.h"
#include "font.h"

static HBViewMode sFinishedView = kHBNoView;

HBViewMode GameViewController::didFinishView() {
	return sFinishedView;
}

GameViewController::GameViewController() {
	sFinishedView = kHBNoView;
	_initMenus();
	_initSound();
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	cout << "Starting client\n";

	addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	getaddrinfo(ipaddy, "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	Socket* sock = new Socket(sockfd);
	sc = sock->connect(*res->ai_addr, res->ai_addrlen);

	int u[2];
	bool done = false;
	for (int i = 0; i < 10 && !done; i++) {
		sc->send();
		SDL_Delay(200);
		int n;
		while ((n = sc->receive((char*)&u, 8)) != -1) {
			if (n == 4) {
				done = true;
				break;
			}
		}
	}
	sc->packetnum = 10;
	if (!done) {
		cout << "Failed to connect" << endl;
		leave();
	}

	int local_order = ntohl(u[0]);
	angle = *reinterpret_cast<float*>(&local_order);

    myId = -1;
}

GameViewController::~GameViewController() {
	sFinishedView = kHBNoView;
	SDL_ShowCursor(true);
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	delete sc;
	delete mainmenu;
}

void GameViewController::process() {
	int status;
	
	int count = 0, oldTime = SDL_GetTicks();
	while ((status = world.receiveObjects(sc, myId)) != -1) {
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

bool GameViewController::quit() {
	_disconnect();
	exit(0);
	return true;
}

bool GameViewController::leave() {
	_disconnect();
	return (sFinishedView = kHBServerConnectView) != 0;
}

static voidtype action_toggle_fullscreen(bool b) {
	if(b) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	}
}

void GameViewController::_disconnect() {
	if (sc) {
		char q = 0;
		sc->add(&q, 1);
		sc->send();
	}
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
	ALCdevice* dev = alcOpenDevice(NULL);
	ALCcontext* con = alcCreateContext(dev, NULL);
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

