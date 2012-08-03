#include "GameViewController.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "UserInput.h"

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
	std::cout << "Starting client\n";

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
			if (rp->message_type == STC_INIT_INFO) {
				done = true;
				break;
			}
            delete rp;
		}
	}
	if (!done) {
		std::cerr << "Failed to connect" << std::endl;
		leave();
	}

	if (done) {
		std::cerr << "Connected to server" << std::endl;

		angle = rp->read_float();
		HBMap hbmap(rp);
		world.reset(new MirroringWorld(hbmap));
		delete rp;
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
			Sound s(rp);
			char c = (char) s.type;
		    float v1 = s.pos.x;
		    float v2 = s.pos.y;

			int src = -1;
			for (int s = 0; s < ALSRCS; s++) {
				int st;
				alGetSourcei(alsrcs[s], AL_SOURCE_STATE, &st);
				if (st != AL_PLAYING) {
					src = s;
					break;
				}
			}
			if (c >= 0 && c < NUM_SOUND_TYPES && src >= 0) {
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
		}
		else {
			latestPacket = rp->packet_number;
			world->readFromPacket(rp);
			delete rp;
		}
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
	UserInput ui(!!keystate[SDLK_a],
	             !!keystate[SDLK_d],
	             !!keystate[SDLK_w],
	             !!keystate[SDLK_s],
	             angle);
	WritePacket wp(CTS_USER_STATE);
	ui.writeToPacket(&wp);
	sc->send_packet(wp);

	Vector2D center(0.0f, 0.0f);
	Vector2D velocity(0.0f, 0.0f);
	if (world->getMyObject() != NULL) {
		center = world->getMyObject()->center;
		velocity = world->getMyObject()->velocity;
	}
	ALfloat alpos[] = { center.x,   center.y,   0 };
	ALfloat alvel[] = { velocity.x, velocity.y, 0 };
	ALfloat alori[] = { 0.0, (ALfloat) cos(angle), (ALfloat) sin(angle), 0.0, 1.0, 0.0 };
	alListenerfv(AL_POSITION, alpos);
	alListenerfv(AL_VELOCITY, alvel);
	alListenerfv(AL_ORIENTATION, alori);
	if (((++count)%100) == 0) {
		int time = SDL_GetTicks();
		float fps = 100000./(time - oldTime);
		printf("\r");
		std::cout.width(6);
		std::cout << (int)fps << "fps" << std::flush;
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

static void action_toggle_fullscreen(bool b) {
	if(b) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	}
}

void GameViewController::_disconnect() {
	if (sc) {
		WritePacket wp(CTS_DISCONNECT, 1);
		sc->send_packet(wp);
	}
}

void GameViewController::_initMenus() {
	mainmenu = new menu();
	mainmenu->add_menuitem(new actionmenuitem([this](){return leave();}, (char *)"Leave Game"));
	mainmenu->add_menuitem(new togglemenuitem((char*)"Fullscreen", false, action_toggle_fullscreen));
	mainmenu->add_menuitem(new actionmenuitem([this](){return quit();}, (char *)"Quit"));

	// first, determine which set of resolutions we should use. 
	//mainmenu->add_menuitem(new slidermenuitem((char *)"Resolution",));
}

void GameViewController::_initSound() {
	ALCdevice* dev = alcOpenDevice(NULL);
	ALCcontext* con = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(con);
	
	pos[0] = 0; pos[1] = 0; pos[2] = 0;
	vel[0] = 0; vel[1] = 0; vel[2] = 0;
	ori[0] = 0.0; ori[1] = 0.0; ori[2] = 1.0; ori[3] = 0.0; ori[4] = 1.0; ori[5] = 0.0;
	
	alutInit(NULL, NULL);
	for (int i = 0; i < NUM_SOUND_TYPES; i++) {
		albuf[i] = alutCreateBufferFromFile(("sounds/" + kSoundFilenames[i] + ".wav").data());
	} 
	alGenSources(ALSRCS, alsrcs);
}
