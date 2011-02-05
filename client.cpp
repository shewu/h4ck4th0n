#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/glew.h>
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

void initVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	
	initGL();
}

/*
 * Event handling:
 * Sends a buffer
 * First byte is event type
 * Followed by appropriate data
 */
int event_handle(void*)
{	
	SDL_Event event;
	for (;;) {
		SDL_WaitEvent(&event);
		switch(event.type) {
			case SDL_KEYDOWN:
			{
				char b = -1;
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						_exit(0);
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
				_exit(0);
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
}

int main(int argc, char* argv[])
{
	initVideo();
	SDL_Thread *thread;
	cout << "Starting client" << endl;
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(argv[1], "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	sock = new Socket(sockfd);
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		cout << "Failed to connect" << endl;
		exit(1);
	}
	angle = 0;
	int v = 0;
	sock->receive((char*)&v, 4);
	myId = ntohl(v);
	int u;
	sock->receive((char*)&u, 4);
	u = ntohl(u);
	angle = *reinterpret_cast<float*>(&u);
	
	thread = SDL_CreateThread(event_handle, NULL);
	
	int otime = 0;
	int i = 0;
	for (;;) {
		do {
			if (!world.receiveObjects(*sock)) exit(1);
		} while (sock->hasRemaining());
		render();
		SDL_GL_SwapBuffers();
		
		if ((++i)%100 == 0) {
			int time = SDL_GetTicks();
			cout << time-otime << endl;
			otime = time;
		}
	}
}
