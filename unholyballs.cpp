#include <iostream>
#include <cstdlib>
#include "unholyrender.h"
#include "hack.h"
#include <SDL/SDL.h>
#include <AL/alut.h>
#include <AL/al.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include <stdint.h>
#include "client.h"
#include "menu.h"

using namespace std;

SDL_Surface *screen;
Socket* sock;
World world;
float angle;
int myId;
unsigned int albuf[3], alsrcs[ALSRCS];
int WIDTH = 640;
int HEIGHT = 480;
char* ipaddy = (char*)"127.0.0.1";
menu *mainmenu;
bool iskeydown[256];
bool NORAPE;

const uint16_t fourbythree[][2] = 
{
	{640, 480}, 
	{800, 600}, 
	{1024, 768}, 
	{1280, 960}, 
	{1400, 1050}, 
	{1600, 1200}, 
	{2048, 1536}
};
const uint16_t sixteenbyten[][2] = 
{
	{800, 500}, 
	{1024, 640}, 
	{1280, 800}, 
	{1440, 900}, 
	{1680, 1050}, 
	{1920, 1200}, 
	{2560, 1600}, 
	{3840, 2400}
};
const uint16_t sixteenbynine[][2] = 
{
	{854, 480}, 
	{1024, 576}, 
	{1280, 720}, 
	{1366, 768}, 
	{1600, 900}, 
	{1920, 1080}, 
	{2560, 1440}
};

#define ALIGNMENT 0x10
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

bool action_quit()
{
	exit(0);
	return true;
}

void action_toggle_fullscreen(bool b)
{
	if(b)
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	return;
}

bool validator_test(char *a) {
	return a[0] == 'a';
}
void initMenus()
{
	mainmenu = new menu();
	menu *menu1 = new menu();
	menu *menu2 = new menu();
	menu1->add_menuitem(new submenuitem(new menu(), (char*)"menu 1 item 1"));
	menu1->add_menuitem(new submenuitem(new menu(), (char*)"menu 1 item 2"));
	menu2->add_menuitem(new submenuitem(new menu(), (char*)"menu 2 item 1"));
	menu2->add_menuitem(new submenuitem(new menu(), (char*)"menu 2 item 2"));

	mainmenu->add_menuitem(new submenuitem(menu1, (char*)"sub menu 1 :)"));
	mainmenu->add_menuitem(new submenuitem(menu2, (char*)"sub menu 2 :)"));
	mainmenu->add_menuitem(new inputmenuitem(20, validator_test, (char *)"", (char*)"Must start with a", (char *)"Enter stuff", (char *)"Stuff"));
	mainmenu->add_menuitem(new togglemenuitem((char*)"Fullscreen", false, action_toggle_fullscreen));
	mainmenu->add_menuitem(new actionmenuitem(action_quit, NULL, (char *)"Quit"));
}

void initVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if(!NORAPE) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
	// detect aspect ratio
	float ratio = (float)SDL_GetVideoInfo()->current_w / SDL_GetVideoInfo()->current_h;

	cout << "SDL_GetVideoInfo()->current_w = " << SDL_GetVideoInfo()->current_w << "\n";
	cout << "SDL_GetVideoInfo()->current_h = " << SDL_GetVideoInfo()->current_h << "\n";
	cout << "aspect ratio = " << ratio << "\n";

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	
	initGL();
}

void initSound()
{
	ALCdevice* dev;
	ALCcontext* con;
	
	dev = alcOpenDevice(NULL);
	con = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(con);
	
	ALfloat pos[] = { 0, 0, 0 };
	ALfloat vel[] = { 0, 0, 0 };
	ALfloat ori[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
	
	alGenBuffers(3, albuf);
	
	alutInit(0,NULL);
	albuf[0] = alutCreateBufferFromFile("sounds/boing2.wav");
	albuf[1] = alutCreateBufferFromFile("sounds/splat2.wav");
	albuf[2] = alutCreateBufferFromFile("sounds/ding.wav");
	alGenSources(ALSRCS, alsrcs);
}

int main(int argc, char* argv[])
{
	initMenus();

	for(int i = 0; i < 256; i++)
		iskeydown[i] = false;

	// process args
	for(int i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-h"))
		{
			printf("Usage:\n"
					"-h to show this message\n"
					"-d [width] [height] to specify viewport dimensions\n"
					"\twhere [width] and [height] are multiples of 16\n"
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
		else if(!strcmp(argv[i], "-norape"))
		{
			NORAPE = true;
		}
	}
	initVideo();
	initSound();
	SDL_Thread *thread;
	cout << "Starting client" << endl;
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(ipaddy, "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	sock = new Socket(sockfd);
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		cout << "Failed to connect" << endl;
		exit(1);
	}
	int u;
	sock->receive((char*)&u, 4);
	u = ntohl(u);
	myId = -1;
	angle = *reinterpret_cast<float*>(&u);

	
	
	int count = 0, oldTime = SDL_GetTicks();
	bool tried_to_get_mouse = false;
	for (;;) {
		while (sock->hasRemaining()) {
			if (!world.receiveObjects(*sock, myId)) exit(1);
			
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
				if (src != -1) {
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
		}
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
				{
					bool isinitialpress = !iskeydown[event.key.keysym.sym];
					iskeydown[event.key.keysym.sym] = true;

					if(mainmenu->is_active())
					{
						break;
					}

					char b = -1;
					switch (event.key.keysym.sym) {
						case SDLK_a:
							b = 0;
							break;
						case SDLK_d:
							b = 1;
							break;
						case SDLK_w:
							b = 2;
							break;
						case SDLK_s:
							b = 3;
							break;
					}
					if (b != -1) {
						sock->send(&b, 1);
					}
					break;
				}
				case SDL_KEYUP:
				{
					iskeydown[event.key.keysym.sym] = false;

					mainmenu->key_input(event.key.keysym.sym);

					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						mainmenu->set_active(!mainmenu->is_active());
						mainmenu->key_input(event.key.keysym.sym);
					}

					if(mainmenu->is_active())
					{
						break;
					}

					char b = -1;
					switch (event.key.keysym.sym) {
						case SDLK_a:
							b = 0;
							break;
						case SDLK_d:
							b = 1;
							break;
						case SDLK_w:
							b = 2;
							break;
						case SDLK_s:
							b = 3;
							break;
					}
					if (b != -1) {
						b ^= 4;
						sock->send(&b, 1);
					}
					break;
				}
				case SDL_QUIT:
				{
					exit(0);
					break;
				}
				case SDL_MOUSEMOTION: {
					int mouse_left_cutoff = 3*WIDTH/8, mouse_right_cutoff = 5*WIDTH/8;
					int mouse_top_cutoff = 3*HEIGHT/8, mouse_bottom_cutoff = 5*HEIGHT/8;
					
					if(SDL_GetAppState() & SDL_APPINPUTFOCUS) {
						if(event.motion.x < mouse_left_cutoff) {
							SDL_WarpMouse(mouse_left_cutoff,event.motion.y);
						}
						if(event.motion.x > mouse_right_cutoff) {
							SDL_WarpMouse(mouse_right_cutoff,event.motion.y);
						}
						if(event.motion.y < mouse_top_cutoff) {
							SDL_WarpMouse(event.motion.x,mouse_top_cutoff);
						}
						if(event.motion.y > mouse_bottom_cutoff) {
							SDL_WarpMouse(event.motion.x,mouse_bottom_cutoff);
						}
					}
					
					if(event.motion.x - event.motion.xrel < mouse_left_cutoff ||
					   event.motion.x - event.motion.xrel > mouse_right_cutoff ||
					   event.motion.y - event.motion.yrel < mouse_top_cutoff ||
					   event.motion.y - event.motion.yrel > mouse_bottom_cutoff) break;
					
					angle -= event.motion.xrel/float(WIDTH);
					while (angle >= 2*M_PI) angle -= 2*M_PI;
					while (angle < 0) angle += 2*M_PI;
					char buf[5];
					buf[0] = 8;
					*((int*)(buf+1)) = htonl(*reinterpret_cast<int*>(&angle));
					sock->send(buf, 5);
					break;
				}
			}
		}

		// If program has focus and mouse isn't inside, move mouse to center of window
		int state = SDL_GetAppState();
		if(state & SDL_APPMOUSEFOCUS) {
			tried_to_get_mouse = false;
		}
		if((state & SDL_APPINPUTFOCUS) && !(state & SDL_APPMOUSEFOCUS) && !tried_to_get_mouse) {
			SDL_WarpMouse(WIDTH/2, HEIGHT/2);
			tried_to_get_mouse = true;
		}
		
		ALfloat alpos[] = { world.objects[myId].p.x, world.objects[myId].p.y, 0 };
		ALfloat alvel[] = { world.objects[myId].v.x, world.objects[myId].v.y, 0 };
		ALfloat alori[] = { 0.0, cos(angle), sin(angle), 0.0, 1.0, 0.0 };
		alListenerfv(AL_POSITION, alpos);
		alListenerfv(AL_VELOCITY, alvel);
		alListenerfv(AL_ORIENTATION, alori);
		
		render();
		if(mainmenu->is_active())
			mainmenu->draw();
		if ((++count)%100 == 0) {
			int time = SDL_GetTicks();
			float fps = 100000./(time - oldTime);
			printf("\b\b\b\b\b\b\b\b\b");
			if(fps < 10) {
				cout << " ";
			}
			if(fps < 100) {
				cout << " ";
			}
			if(fps < 1000) {
				cout << " ";
			}
			if(fps < 10000) {
				cout << " ";
			}
			if(fps < 100000) {
				cout << " ";
			}
			cout << (int)fps << "fps";
			oldTime = time;
			fflush(stdout);
		}
		SDL_GL_SwapBuffers();
	}
	cout << "\n"; // weird, why isn't this printing?
	fflush(stdout);
	SDL_Quit();
}
