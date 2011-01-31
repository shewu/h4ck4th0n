#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include <SDL/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;

SDL_Surface *screen;

int initVideo()
{
	Uint32 flags = SDL_DOUBLEBUF | SDL_FULLSCREEN;
	atexit(SDL_Quit);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
	return true;
}

int main(int argc, char* argv[])
{
	Socket sock(0);
	World w;
	initVideo();
	for (;;) {
		do {
			render();
			w.receiveObjects(sock);
		} while (sock.hasRemaining());
	}
}
