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
#include "constants.h"
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
				char buf[] = {0, 0};
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						exit(0);
						break;
					case SDLK_LEFT:
						buf[1] = 1;
						break;
					case SDLK_RIGHT:
						buf[1] = 2;
						break;
					case SDLK_UP:
						buf[1] = 3;
						break;
					case SDLK_DOWN:
						buf[1] = 4;
						break;
				}
				if (buf[1]) {
					sock->send(buf, 2);
				}
				break;
			}
			case SDL_KEYUP:
			{
				char buf[] = {1, 0};
				switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						buf[1] = 1;
						break;
					case SDLK_RIGHT:
						buf[1] = 2;
						break;
					case SDLK_UP:
						buf[1] = 3;
						break;
					case SDLK_DOWN:
						buf[1] = 4;
						break;
				}
				if (buf[1]) sock->send(buf, 2);
				break;
			}
			case SDL_QUIT:
			{
				exit(0);
				break;
			}
			case SDL_MOUSEMOTION:
				angle -= event.motion.xrel/float(WIDTH);
			default:
			{
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

	/*	
	Object o1;
	o1.p = Vector2D(-2, -2);
	o1.v = Vector2D(1, 1);
	o1.mass = 1;
	o1.rad = 1;
	o1.h = 2;
	o1.id = 0;
	o1.color = Color(1, 0, 0);
	world.objects[0] = o1;
	
	Object o2;
	o2.p = Vector2D(0, 0);
	o2.v = Vector2D(0, 1);
	o2.mass = 1;
	o2.rad = 1;
	o2.h = 2;
	o2.id = 1;
	o2.color = Color(0, 1, 0);
	world.objects[1] = o2;
	
	Obstacle ob1;
	ob1.p1 = Vector2D(0, 3);
	ob1.p2 = Vector2D(0, 2);
	ob1.color = Color(0, 0, 1);
	world.obstacles.push_back(ob1);*/
	
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
	cout << myId << endl;
	
	thread = SDL_CreateThread(event_handle, NULL);
	
	for (;;) {
		render();
		do {
			if (!world.receiveObjects(*sock)) exit(1);
		} while (sock->hasRemaining());
		SDL_GL_SwapBuffers();
	}
}
