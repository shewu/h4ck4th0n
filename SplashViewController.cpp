#include <cstring>
#include <cstdio>

static void menuFinishedView() {
	finishedView = true;
}

static void menuQuit() {
	printf("Exiting\n");
	exit(0);
}

SplashViewController::SplashViewController() {
	finishedView = false;

	splashMenu = new menu();
	splashMenu->addmenuitem(new actionmenuitem(menuFinishedView, NULL, (char *)"Play Game"));
	splashMenu->addmenuitem(new actionmenuitem(menuQuit, NULL, (char *)"Quit"));

	splashMenu->set_active(true);
}

SplashViewController::~SplashViewController() {
	delete splashMenu;
}

bool SplashViewController::didFinishView() {
	return finishedView;
}

void SplashViewController::process() {
	splashMenu->drawMenu();
	return;
}

void SplashViewController::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFlush();
}

