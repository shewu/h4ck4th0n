#include <cstring>
#include <cstdio>
#include <SDL2/SDL.h>
#ifndef __APPLE__
#include <GL/glew.h>
#include <GL/glu.h>
#else
#include <OpenGL/glu.h>
#endif

#include "SplashViewController.h"
#include "menufuncs.h"

static HBViewMode finishedView = kHBNoView;

bool SplashViewController::quit() {
	exit(0);
	return true;
}

static bool menuConnectToServer(voidtype) {
	finishedView = kHBServerConnectView;
	return true;
}
SplashViewController::SplashViewController() {
    SDL_SetWindowGrab(screen, SDL_FALSE);
	finishedView = kHBNoView;

	splashMenu = new menu();
	splashMenu->add_menuitem(new actionmenuitem(menuConnectToServer, "Play Game"));
	splashMenu->add_menuitem(new actionmenuitem(quitfunc(this), "Quit"));

	splashMenu->set_active(true);
}

SplashViewController::~SplashViewController() {
	finishedView = kHBNoView;
	delete splashMenu;
}

HBViewMode SplashViewController::didFinishView() {
	return finishedView;
}

void SplashViewController::process() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYUP:
				splashMenu->key_input(event.key.keysym.sym);
				break;
		}
	}
	return;
}

void SplashViewController::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	splashMenu->drawMenu();
	glFlush();
}

