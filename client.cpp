#include <iostream>
#include <cstdlib>
#include "render.h"
#include "hack.h"
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <GL/glew.h>
#include "constants.h"
#include "client.h"

using namespace std;

SDL_Surface *screen;
Socket sock(1);
World world;

void initVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	
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
				char buf[] = {(char) SDL_KEYDOWN, (char) event.key.keysym.sym};
				//sock.send(buf, 2);
				break;
			}
			case SDL_KEYUP:
			{
				char buf[] = {(char) SDL_KEYUP, (char) event.key.keysym.sym};
				//sock.send(buf, 2);
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
	*/
	
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
	world.obstacles.push_back(ob1);
	
	for (;;) {
		render();
		/*do {
			world.receiveObjects(sock);
		} while (sock.hasRemaining());*/
		world.doSimulation(.0005);
		SDL_GL_SwapBuffers();
	}
}
