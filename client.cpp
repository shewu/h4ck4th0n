#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include <SDL/SDL.h>
#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include "client.h"
#include "menu.h"
#include "font.h"

using namespace std;

int WIDTH = -1;
int HEIGHT = -1;

SDL_Surface* screen;
SocketConnection* sc;
World world;
float angle;
int myId;
unsigned int albuf[3], alsrcs[ALSRCS];
char* ipaddy = (char*)"18.248.6.168";
menu* mainmenu;

#ifdef UNHOLY
bool NORAPE;
#endif

void quit() {
	// check for null pointers!
	if(sc) {
		char q = 0;
		sc->add(&q, 1);
		sc->send();
	}
	exit(0);
}

bool action_quit() {
	quit();
	return true;
}

void action_toggle_fullscreen(bool b) {
	if(b)
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	return;
}

bool validator_test(char *a) {
	return a[0] == 'a';
}

char * state [] = {(char*)"abc", (char*)"bad", (char*)"cat!"};

void initMenus() {
	mainmenu = new menu();
	menu *menu1 = new menu();
	menu *menu2 = new menu();
	menu1->add_menuitem(new submenuitem(new menu(), (char*)"menu 1 item 1"));
	menu1->add_menuitem(new submenuitem(new menu(), (char*)"menu 1 item 2"));
	menu2->add_menuitem(new submenuitem(new menu(), (char*)"menu 2 item 1"));
	menu2->add_menuitem(new submenuitem(new menu(), (char*)"menu 2 item 2"));

	mainmenu->add_menuitem(new submenuitem(menu1, (char*)"sub menu 1 :)"));
	mainmenu->add_menuitem(new submenuitem(menu2, (char*)"sub menu 2 :)"));
//	char * state [] = {(char*)"abc", (char*)"bad", (char*)"cat!"};
	mainmenu->add_menuitem(new slidermenuitem((char*)"Slider!", state, 3, 0, NULL));
	//mainmenu->add_menuitem(new inputmenuitem(20, validator_test, (char *)"", (char*)"Must start with a", (char *)"Enter stuff", (char *)"Stuff"));
#ifndef __APPLE__
	mainmenu->add_menuitem(new togglemenuitem((char*)"Fullscreen", false, action_toggle_fullscreen));
#endif
	mainmenu->add_menuitem(new actionmenuitem(action_quit, NULL, (char *)"Quit"));
}

void initVideo() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef UNHOLY
	if(!NORAPE)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
#endif
	// detect aspect ratio
	float ratio = (float)SDL_GetVideoInfo()->current_w / SDL_GetVideoInfo()->current_h;

	float d16x9 = abs(ratio - SIXTEEN_BY_NINE);
	float d16x10 = abs(ratio - SIXTEEN_BY_TEN);
	float d4x3 = abs(ratio - FOUR_BY_THREE);

	if(WIDTH == -1 || HEIGHT == -1)
	{
		if(d16x9 < d16x10 && d16x9 < d4x3)
		{
			WIDTH = sixteenbynine[0][0];
			HEIGHT = sixteenbynine[0][1];
		}
		else if(d16x10 < d16x9 && d16x10 < d4x3)
		{
			WIDTH = sixteenbyten[0][0];
			HEIGHT = sixteenbyten[0][1];
		}
		else if(d4x3 < d16x10 && d4x3 < d16x9)
		{
			WIDTH = fourbythree[0][0];
			HEIGHT = fourbythree[0][1];
		}
		else
		{
			WIDTH = 640;
			HEIGHT = 480;
		}
	}

	WIDTH = ALIGN(WIDTH);
	HEIGHT = ALIGN(HEIGHT);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	
	initGL();

	cout << "using resolution " << WIDTH << "x" << HEIGHT << "\n";
	return;
}

void initSound()
{
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

int main(int argc, char* argv[])
{
	srand(time(NULL));
	// process args
	for(int i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-h"))
		{
			printf("Usage:\n"
					"-h to show this message\n"
					"-d [width] [height] to specify viewport dimensions\n"
					"\twhere [width] and [height] are multiples of 16\n"
#ifdef UNHOLY
					"-norape to avoid antialiasing\n"
#endif
					"-i [ip] to connect to specified server\n");
			exit(0);
		}
		else if(!strcmp(argv[i], "-d"))
		{
			// round up to next highest multiple of 16 if not already a multiple
			// of 16
			WIDTH = ALIGN(atoi(argv[i+1]));
			HEIGHT = ALIGN(atoi(argv[i+2]));
			cout << "Playing at " << WIDTH << "x" << HEIGHT << "\n";
		}
		else if(!strcmp(argv[i], "-i"))
		{
			ipaddy = argv[i+1];
		}
#ifdef UNHOLY
		else if(!strcmp(argv[i], "-norape"))
		{
			NORAPE = true;
		}
#endif
	}
	
	initVideo();
	initSound();
	initMenus();
	init_font();
	SDL_Thread *thread;
	cout << "Starting client" << endl;
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	getaddrinfo(ipaddy, "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	Socket sock(sockfd);
	sc = sock.connect(*res->ai_addr, res->ai_addrlen);
	
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
		exit(1);
	}
	
	u[0] = ntohl(u[0]);
	myId = -1;
	angle = *reinterpret_cast<float*>(u);
	
	int count = 0, oldTime = SDL_GetTicks();
	for (;;) {
		int status;
		while ((status = world.receiveObjects(sc, myId)) != -1) {
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
				{
					if(event.key.keysym.sym == SDLK_ESCAPE) {
						if (mainmenu->is_active()) {
							SDL_ShowCursor(false);
							SDL_WM_GrabInput(SDL_GRAB_ON);
							mainmenu->set_active(false);
						}
						else {
							SDL_ShowCursor(true);
							SDL_WM_GrabInput(SDL_GRAB_OFF);
							mainmenu->set_active(true);
						}
					}
					mainmenu->key_input(event.key.keysym.sym);
					break;
				}
				case SDL_QUIT:
				{
					quit();
					break;
				}
				case SDL_MOUSEMOTION: {
					if (mainmenu->is_active()) break;
					angle -= event.motion.xrel/400.0;

					while (angle >= 2*M_PI) angle -= 2*M_PI;
					while (angle < 0) angle += 2*M_PI;
					break;
				}
			}
		}
		
		Uint8* keystate = SDL_GetKeyState(NULL);
		char buf[5];
		*((int*)buf) = htonl(*reinterpret_cast<int*>(&angle));
		buf[4] = 0;
		if(!mainmenu->is_active())
		{
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
		
		render();
		if (mainmenu->is_active()) mainmenu->drawMenu();
		int time = SDL_GetTicks();
		if ((++count)%100 == 0) {
			int time = SDL_GetTicks();
			float fps = 100000./(time - oldTime);
			printf("\r");
			cout.width(6);
			cout << (int)fps << "fps" << flush;
			oldTime = time;
		}
		
		SDL_GL_SwapBuffers();
	}
	cout << endl;
	SDL_Quit();
}
