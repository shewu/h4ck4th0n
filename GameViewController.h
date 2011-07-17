#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
#endif

#include "HBViewController.h"
#include "hack.h"
#include "constants.h"
#include "menu.h"
#include "font.h"

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

	public:
		GameViewController();
		~GameViewController();
};

