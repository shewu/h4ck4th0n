#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
#include <GL/glew.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "HBViewController.h"
#include "hack.h"
#include "constants.h"
#include "menu.h"
#include "font.h"

extern SDL_Surface* screen;
extern char* ipaddy;
extern int WIDTH;
extern int HEIGHT;
#ifdef UNHOLY
extern bool NORAPE;
#endif

class GameViewController : public HBViewController {
	private:
		unsigned int albuf[3], alsrcs[ALSRCS];
		SocketConnection* sc;
		World world;
		float angle;
		int myId;
		menu* mainmenu;
		int program;
		GLUquadric* quad;
		void _initGL();
		void _initMenus();
		void _initSound();
		void _drawWalls();
		void _drawObjects();
		void _drawFloor(float);

	public:
		GameViewController();
		~GameViewController();

		bool didFinishView();
		void process();
		void render();
};

