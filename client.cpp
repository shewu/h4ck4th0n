#include <iostream>
#include "render.h"
#include "hack.h"
#include "SDL.h"

using namespace std;

SDL_Surface *screen;

int main(int argc, char* argv[])
{
	Socket sock(0);
	World w;
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
	for (;;) {
		do {
			render();
			w.receiveObjects(sock);
		} while (sock.hasRemaining());
	}
}
