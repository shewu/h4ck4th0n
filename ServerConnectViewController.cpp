#include <cstring>
#include <cstdio>
#ifndef __APPLE__
#include <GL/glew.h>
#include <GL/glu.h>
#else
#include <OpenGL/glu.h>
#endif

#include "ServerConnectViewController.h"
#include "menufuncs.h"

extern char* ipaddy;
static HBViewMode finishedView = kHBNoView;

static bool menuGetIP(char* a) {
	// note: strcmp returns 0 if equal
	ipaddy = a;
	printf("Entered IP = %s\n", a);
	if (strcmp(a, "")) {
		return (finishedView = kHBGameView) != kHBNoView;
	} else {
		return false;
	}
}

bool ServerConnectViewController::quit() {
	exit(0);
	return true;
}

static bool goBack(voidtype) {
	finishedView = kHBMainMenuView;
	return true;
}

ServerConnectViewController::ServerConnectViewController() {
    SDL_SetWindowGrab(screen, SDL_FALSE);
	finishedView = kHBNoView;

	serverConnectMenu = new menu();
	serverConnectMenu->add_menuitem(new inputmenuitem(20, menuGetIP, (char *)"", (char *)"Must not be empty", (char *)"Enter Server IP Address", "Connect to server"));
	serverConnectMenu->add_menuitem(new actionmenuitem(goBack, "Back"));
	serverConnectMenu->add_menuitem(new actionmenuitem(quitfunc(this), "Quit"));

	serverConnectMenu->set_active(true);
}

ServerConnectViewController::~ServerConnectViewController() {
	finishedView = kHBNoView;
	delete serverConnectMenu;
}

HBViewMode ServerConnectViewController::didFinishView() {
	return finishedView;
}

void ServerConnectViewController::process() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYUP:
				serverConnectMenu->key_input(event.key.keysym.sym);
				break;
		}
	}
	return;
}

void ServerConnectViewController::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	serverConnectMenu->drawMenu();
	glFlush();
}

