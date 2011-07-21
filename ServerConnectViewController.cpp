#include <cstring>
#include <cstdio>
#ifndef __APPLE__
#include <GL/glew.h>
#include <GL/glu.h>
#endif

#include "ServerConnectViewController.h"

extern char* ipaddy;
static bool finishedView = false;

static bool menuGetIP(char* a) {
	// note: strcmp returns 0 if equal
	ipaddy = a;
	printf("Entered IP = %s\n", a);
	return finishedView = strcmp(a, "");
}

static bool menuQuit() {
	// we need to deallocate memory
	printf("Exiting\n");
	exit(0);
	return true;
}

ServerConnectViewController::ServerConnectViewController() {
	finishedView = false;

	serverConnectMenu = new menu();
	serverConnectMenu->add_menuitem(new inputmenuitem(20, menuGetIP, (char *)"", (char *)"Must not be empty", (char *)"Enter Server IP Address", (char *)"Stuff"));
	serverConnectMenu->add_menuitem(new actionmenuitem(menuQuit, NULL, (char *)"Quit"));

	serverConnectMenu->set_active(true);
}

ServerConnectViewController::~ServerConnectViewController() {
	delete serverConnectMenu;
}

bool ServerConnectViewController::didFinishView() {
	return finishedView;
}

void ServerConnectViewController::process() {
	serverConnectMenu->drawMenu();
	return;
}

void ServerConnectViewController::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFlush();
}

