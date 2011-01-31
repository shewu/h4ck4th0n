#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/glew.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;

SDL_Surface *screen;
Socket sock(1);
World world;

void initVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 24, SDL_DOUBLEBUF | SDL_OPENGL);
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
	for (;;){
		SDL_WaitEvent(&event);
		switch(event.type) {
			case SDL_KEYDOWN:
			{
				char buf[] = {(char) SDL_KEYDOWN, (char) event.key.keysym.sym};
				sock.send(buf, 2);
				break;
			}
			case SDL_KEYUP:
			{
				char buf[] = {(char) SDL_KEYUP, (char) event.key.keysym.sym};
				sock.send(buf, 2);
				break;
			}
			case SDL_QUIT:
			{
				exit(0);
				break;
			}
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
	thread = SDL_CreateThread(event_handle, NULL);
	cout << "Starting client" << endl;
	for (;;) {
		do {
			render();
			world.receiveObjects(sock);
		} while (sock.hasRemaining());
	}
}
