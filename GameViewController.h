#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
#endif

#include "menu.h"
#include "font.h"
#include "hack.h"

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

	public:
		GameViewController();
		~GameViewController();
};

