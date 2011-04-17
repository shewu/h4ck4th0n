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
SocketConnection* sc;
World world;
float angle;
int myId;
unsigned int albuf[3], alsrcs[ALSRCS];

void initVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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
	albuf[0] = alutCreateBufferFromFile("boing.wav");
	albuf[1] = alutCreateBufferFromFile("splat.wav");
	albuf[2] = alutCreateBufferFromFile("ding.wav");
	alGenSources(ALSRCS, alsrcs);
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	initVideo();
	initSound();
	SDL_Thread *thread;
	cout << "Starting client" << endl;
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	getaddrinfo(argv[1], "55555", &hints, &res);
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
	int ooldTime = oldTime;
	for (;;) {
		int status;
		while ((status = world.receiveObjects(sc, myId)) != -1) {
			if (status == 0) continue;
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
					if (event.key.keysym.sym != SDLK_ESCAPE) break;
				}
				case SDL_QUIT:
				{
					char q = 0;
					sc->add(&q, 1);
					sc->send();
					exit(0);
					break;
				}
				case SDL_MOUSEMOTION: {
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
		if (keystate[SDLK_a]) buf[4] ^= 1;
		if (keystate[SDLK_d]) buf[4] ^= 2;
		if (keystate[SDLK_w]) buf[4] ^= 4;
		if (keystate[SDLK_s]) buf[4] ^= 8;
		sc->add(buf, 5);
		
		ALfloat alpos[] = { world.objects[myId].p.x, world.objects[myId].p.y, 0 };
		ALfloat alvel[] = { world.objects[myId].v.x, world.objects[myId].v.y, 0 };
		ALfloat alori[] = { 0.0, cos(angle), sin(angle), 0.0, 1.0, 0.0 };
		alListenerfv(AL_POSITION, alpos);
		alListenerfv(AL_VELOCITY, alvel);
		alListenerfv(AL_ORIENTATION, alori);
		
		render();
		int time = SDL_GetTicks();
		if ((++count)%100 == 0) {
			cout << "100 frames in " << time-oldTime << " milliseconds" << endl;
			oldTime = time;
		}
		
		if (time-ooldTime > 500) sc->send();
		
		SDL_GL_SwapBuffers();
	}
}
