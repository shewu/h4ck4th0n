#include <cstring>
#include <cstdio>

#include "menu.h"
#include "font.h"

extern char* ipaddy;

class ServerConnectViewConroller : public HBViewController {
	public:
		menu* serverConnectMenu;
		ServerConnectViewConroller();
		~ServerConnectViewConroller();
		/* tell the menu to draw. */
		void process();
		/* draw a black background for now. */
		void render();
};

static bool menuGetIP(char* a) {
	// note: strcmp returns 0 if equal
	ipaddy = a;
	printf("Entered IP = %s\n", a);
	return strcmp(a, "");
}

static void menuQuit() {
	// we need to deallocate memory
	printf("Exiting\n");
	exit(0);
}

ServerConnectViewConroller::ServerConnectViewConroller() {
	serverConnectMenu = new menu();
	serverConnectMenu->add_menuitem(new inputmenuitem(20, menuGetIP, (char *)"", (char *)"Must not be empty", (char *)"Enter Server IP Address", (char *)"Stuff"));
	serverConnectMenu->add_menuitem(new actionmenuitem(menuQuit, NULL, (char *)"Quit"));

	serverConnectMenu->set_active(true);
}

ServerConnectViewConroller::~ServerConnectViewConroller() {
	delete serverConnectMenu;
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

