#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include <SDL/SDL.h>
#include <AL/alut.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include "client.h"

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
bool FULLSCREEN;

#define ALIGNMENT 0x10
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

void initVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if(FULLSCREEN) 
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	else
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	
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
	
	alutInit(NULL, NULL);
	albuf[0] = alutCreateBufferFromFile("sounds/boing2.wav");
	albuf[1] = alutCreateBufferFromFile("sounds/splat2.wav");
	albuf[2] = alutCreateBufferFromFile("sounds/ding.wav");
	alGenSources(ALSRCS, alsrcs);
}

int main(int argc, char* argv[])
{
	// process args
	for(int i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-h"))
		{
			printf("Usage:\n"
					"-h to show this message\n"
					"-f for fullscreen\n"
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
		else if(!strcmp(argv[i], "-f"))
		{
			FULLSCREEN = true;
		}
		else if(!strcmp(argv[i], "-i"))
		{
			ipaddy = argv[i+1];
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
		}
		
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
				{
					char b = -1;
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							exit(0);
							break;
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
		
		ALfloat alpos[] = { world.objects[myId].p.x, world.objects[myId].p.y, 0 };
		ALfloat alvel[] = { world.objects[myId].v.x, world.objects[myId].v.y, 0 };
		ALfloat alori[] = { 0.0, cos(angle), sin(angle), 0.0, 1.0, 0.0 };
		alListenerfv(AL_POSITION, alpos);
		alListenerfv(AL_VELOCITY, alvel);
		alListenerfv(AL_ORIENTATION, alori);
		
		render();
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
