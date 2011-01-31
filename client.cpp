#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include "SDL.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace std;

SDL_Surface *screen;

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

int initVideo()
{
	Uint32 flags = SDL_DOUBLEBUF | SDL_FULLSCREEN;
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf("Error");
		return false;
	}
	atexit(SDL_Quit);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
	if (screen == NULL) {
		fprintf("Error");
		return false;
	}
	return true;
}
