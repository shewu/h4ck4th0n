#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "GameViewController.h"
#include "MenuFuncs.h"

#define MAX_SOUND_LATENESS 100

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
	
	printf("IP Address = %s\n", ipaddy);
	getaddrinfo(ipaddy, "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	sock = new Socket(sockfd);

	sc = sock->connect(res->ai_addr, res->ai_addrlen);

	bool done = false;
	ReadPacket* rp;
	for (int i = 0; i < 10 && !done; i++) {
		WritePacket wp(CTS_CONNECT, 0); 
        sc->send_packet(wp);

		SDL_Delay(200);
		sock->recv_all();
		while ((rp = sc->receive_packet()) != NULL) {
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

	if (done) {
		cout << "Connected to server" << endl;

		angle = rp->read_float();
		HBMap hbmap(rp);
		world = MirroringWorld(hbmap)
		delete rp;

		myId = -1;
	}
	latestPacket = 0;

	count = 0;
	oldTime = SDL_GetTicks();
}

GameViewController::~GameViewController() {
	sFinishedView = kHBNoView;
	SDL_ShowCursor(true);
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	delete sc;
	delete mainmenu;
}

void GameViewController::process() {
	sock->recv_all();
	while(1) {
		ReadPacket *rp = sc->receive_packet();
		if(rp == NULL)
			break;
		if (rp->message_type == STC_SOUND && rp->packet_number >= latestPacket - MAX_SOUND_LATENESS) {
			char c = rp->read_char();
		    int v1 = rp->read_float();
		    int v2 = rp->read_float();

			int src = -1;
			for (int s = 0; s < ALSRCS; s++) {
				int st;
				alGetSourcei(alsrcs[s], AL_SOURCE_STATE, &st);
				if (st != AL_PLAYING) {
					src = s;
					break;
				}
			}
			if (c >= 0 && c < 3 && src >= 0) {
				ALfloat alsrcpos[] = { v1, v2, 0 };
				ALfloat alsrcvel[] = { 0, 0, 0 };

				alSourcef(alsrcs[src], AL_PITCH, 1.0f);
				alSourcef(alsrcs[src], AL_GAIN, 1.0f);
				alSourcefv(alsrcs[src], AL_POSITION, alsrcpos);
				alSourcefv(alsrcs[src], AL_VELOCITY, alsrcvel);
				alSourcei(alsrcs[src], AL_BUFFER, albuf[(int)c]);
				alSourcei(alsrcs[src], AL_LOOPING, AL_FALSE);
				alSourcePlay(alsrcs[src]);
			}
		}
		else if (rp->message_type != STC_WORLD_DATA || rp->packet_number <= latestPacket) {
			delete rp;
			continue;
		}
		latestPacket = rp->packet_number;
		world.receiveObjects(rp, myId);
		delete rp;
#ifndef __APPLE__
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
	WritePacket wp(CTS_USER_STATE);
	wp.write_float(angle);
	char keystate_byte = 0;
	if(!mainmenu->is_active()) {
		if (keystate[SDLK_a]) keystate_byte ^= 1;
		if (keystate[SDLK_d]) keystate_byte ^= 2;
		if (keystate[SDLK_w]) keystate_byte ^= 4;
		if (keystate[SDLK_s]) keystate_byte ^= 8;
	}
	wp.write_char(keystate_byte);
	sc->send_packet(wp);
	
#ifndef __APPLE__
	ALfloat alpos[] = { world.objects[myId].p.x, world.objects[myId].p.y, 0 };
	ALfloat alvel[] = { world.objects[myId].v.x, world.objects[myId].v.y, 0 };
	ALfloat alori[] = { 0.0, cos(angle), sin(angle), 0.0, 1.0, 0.0 };
	alListenerfv(AL_POSITION, alpos);
	alListenerfv(AL_VELOCITY, alvel);
	alListenerfv(AL_ORIENTATION, alori);
#endif
	if (((++count)%100) == 0) {
		int time = SDL_GetTicks();
		float fps = 100000./(time - oldTime);
		printf("\r");
		cout.width(6);
		cout << (int)fps << "fps" << flush;
		oldTime = time;
	}
}

bool GameViewController::quit() {
	_disconnect();
	exit(0);
	return true;
}

bool GameViewController::leave() {
	_disconnect();
	return (sFinishedView = kHBServerConnectView) != kHBNoView;
}

static voidtype action_toggle_fullscreen(bool b) {
	if(b) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	}
	return voidtype();
}

void GameViewController::_disconnect() {
	if (sc) {
		WritePacket wp(CTS_DISCONNECT, 1);
		sc->send_packet(wp);
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
	
	pos[0] = 0; pos[1] = 0; pos[2] = 0;
	vel[0] = 0; vel[1] = 0; vel[2] = 0;
	ori[0] = 0.0; ori[1] = 0.0; ori[2] = 1.0; ori[3] = 0.0; ori[4] = 1.0; ori[5] = 0.0;
	
	alutInit(NULL, NULL);
	albuf[0] = alutCreateBufferFromFile("sounds/boing2.wav");
	albuf[1] = alutCreateBufferFromFile("sounds/splat2.wav");
	albuf[2] = alutCreateBufferFromFile("sounds/ding.wav");
	alGenSources(ALSRCS, alsrcs);
#endif
}

