#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
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

class GameViewController : public HBViewController {
	private:
		unsigned int albuf[3], alsrcs[ALSRCS];
		SocketConnection* sc;
		World world;
		float angle;
		int myId;
		menu* mainmenu;
#ifdef UNHOLY
		bool NORAPE;
#endif
		void initGL();
		void initMenus();
		void initSound();

	public:
		GameViewController();
		~GameViewController();

		bool didFinishView();
		void process();
		void render();
};

