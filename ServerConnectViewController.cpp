#include <cstring>
#include <cstdio>

#include "menu.h"
#include "font.h"
#include "HBViewController.h"

extern char* ipaddy;

class ServerConnectViewConroller : public HBViewController {
	private:
		menu* serverConnectMenu;
		bool finishedView;
	public:
		bool didFinishView();
		ServerConnectViewConroller();
		~ServerConnectViewConroller();
};

static bool menuGetIP(char* a) {
	// note: strcmp returns 0 if equal
	ipaddy = a;
	printf("Entered IP = %s\n", a);
	return finishedView = strcmp(a, "");
}

static void menuQuit() {
	// we need to deallocate memory
	printf("Exiting\n");
	exit(0);
}

ServerConnectViewConroller::ServerConnectViewConroller() {
	finishedView = false;

	serverConnectMenu = new menu();
	serverConnectMenu->add_menuitem(new inputmenuitem(20, menuGetIP, (char *)"", (char *)"Must not be empty", (char *)"Enter Server IP Address", (char *)"Stuff"));
	serverConnectMenu->add_menuitem(new actionmenuitem(menuQuit, NULL, (char *)"Quit"));

	serverConnectMenu->set_active(true);
}

ServerConnectViewConroller::~ServerConnectViewConroller() {
	delete serverConnectMenu;
}

bool ServerConnectViewConroller::didFinishView() {
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

