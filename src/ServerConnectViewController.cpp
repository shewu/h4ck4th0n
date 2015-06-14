#include <cstring>
#include <cstdio>
#ifndef __APPLE__
#include <GL/glew.h>
#include <GL/glu.h>
#else
#include <OpenGL/glu.h>
#endif

#include "ServerConnectViewController.h"

using std::string;

extern string ipaddy;
static HBViewMode finishedView = kHBNoView;

static bool menuGetIP(string const& ip) {
	ipaddy = ip;
	printf("Entered IP = %s\n", ipaddy.c_str());
	if (ip != "") {
		return (finishedView = kHBGameView) != kHBNoView;
	} else {
		return false;
	}
}

bool ServerConnectViewController::quit() {
	exit(0);
	return true;
}

static bool goBack() {
	finishedView = kHBMainMenuView;
	return true;
}

ServerConnectViewController::ServerConnectViewController() {
    SDL_SetWindowGrab(screen, SDL_FALSE);
	finishedView = kHBNoView;

	serverConnectMenu = new Menu();
	serverConnectMenu->addMenuItem(new InputMenuItem(20, menuGetIP, (char *)"", (char *)"Must not be empty", (char *)"Enter Server IP Address", (char *)"Connect to server"));
	serverConnectMenu->addMenuItem(new ActionMenuItem(goBack, (char *)"Back"));
	serverConnectMenu->addMenuItem(new ActionMenuItem([this](){return quit();}, (char *)"Quit"));

	serverConnectMenu->setActive(true);
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
				serverConnectMenu->keyInput(event.key.keysym.sym);
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

