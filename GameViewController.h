#ifndef GAMEVIEWCONTROLLER_H
#define GAMEVIEWCONTROLLER_H

#include "HBViewController.h"

#include <memory>

#include "AL/alut.h"

#include "Menu.h"
#include "MirroringWorld.h"
#include "Packet.h"
#include "Socket.h"
#include "Sound.h"

extern SDL_Surface* screen;
extern char* ipaddy;
extern int WIDTH;
extern int HEIGHT;

class GameViewController : public HBViewController {
	public:
		GameViewController();
		~GameViewController();
		HBViewMode didFinishView();
		void process();
		void render() {}
		bool quit();
		bool leave();

	protected:
		unsigned int albuf[NUM_SOUND_TYPES], alsrcs[ALSRCS];
		Socket *sock;
		SocketConnection* sc;
		std::unique_ptr<MirroringWorld> world;
		float angle;
		int latestPacket;
		int count;
		int oldTime;
		menu* mainmenu;
		menu* _resmenu;

		ALfloat pos[3], vel[3], ori[6];

	private:
		void _disconnect();
		void _initMenus();
		void _initSound();
};

class leavefunc {
	public:
		GameViewController* _gvc;
		leavefunc() {}
		leavefunc(GameViewController* gvc) {
			_gvc = gvc;
		}
		bool operator()() {
			return _gvc->leave();
		}
};

#endif
