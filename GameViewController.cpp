#include "GameViewController.h"

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>

#include "UserInput.h"
#include "Util.h"

#define MAX_SOUND_LATENESS 100

static HBViewMode sFinishedView = kHBNoView;
static unsigned sVideoModeFlags = SDL_OPENGL;

extern int WIDTH;
extern int HEIGHT;

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
	
	std::cout << "IP Address = " << ipaddy << "\n";
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
	delete _resmenu;
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
					if (mainmenu->isActive()) {
						SDL_ShowCursor(false);
						SDL_WM_GrabInput(SDL_GRAB_ON);
						mainmenu->setActive(false);
					} else {
						SDL_ShowCursor(true);
						SDL_WM_GrabInput(SDL_GRAB_OFF);
						mainmenu->setActive(true);
					}
				}
				mainmenu->keyInput(event.key.keysym.sym);
				break;
			case SDL_QUIT:
				quit();
				break;
			case SDL_MOUSEMOTION:
				if (mainmenu->isActive()) break;
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

static void actionToggleFullscreen(bool b) {
	sVideoModeFlags = (b) ? SDL_OPENGL | SDL_FULLSCREEN : SDL_OPENGL;
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, sVideoModeFlags);
}

void GameViewController::_disconnect() {
	if (sc) {
		WritePacket wp(CTS_DISCONNECT, 1);
		sc->send_packet(wp);
	}
}

void GameViewController::_initMenus() {
	mainmenu = new Menu();
	mainmenu->addMenuItem(new ActionMenuItem([this](){return leave();}, (char *)"Leave Game"));
	mainmenu->addMenuItem(new ToggleMenuItem((char*)"Fullscreen", false, actionToggleFullscreen));

	// first, determine which set of resolutions we should use. 
	const float ratio = float(SDL_GetVideoInfo()->current_w) / SDL_GetVideoInfo()->current_h;
	const float d5x4 = fabs(ratio - FIVE_BY_FOUR);
	const float d4x3 = fabs(ratio - FOUR_BY_THREE);
	const float d16x10 = fabs(ratio - SIXTEEN_BY_TEN);
	const float d16x9 = fabs(ratio - SIXTEEN_BY_NINE);
	const int arg = argMin({d5x4, d4x3, d16x10, d16x9});

	_resmenu = new Menu();
	for (auto p : RESOLUTIONS[arg]) {
		std::ostringstream resStream;
		resStream << p.first << " x " << p.second;
		_resmenu->addMenuItem(new ActionMenuItem(
			[p]() {
				WIDTH = p.first;
				HEIGHT = p.second;
				SDL_SetVideoMode(p.first, p.second, 24, sVideoModeFlags);
				glViewport(0, 0, p.first, p.second);
				return 0;
			},
			(char *)(resStream.str().c_str())
		));
	}
	mainmenu->addMenuItem(new SubMenuItem(_resmenu, (char *)"Resolution"));
	mainmenu->addMenuItem(new ActionMenuItem([this](){return quit();}, (char *)"Quit"));
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
