#ifndef GAMEVIEWCONTROLLER_H
#define GAMEVIEWCONTROLLER_H

#include "HBViewController.h"
#include "Hack.h"
#include "Menu.h"
#include "Packet.h"
#include "MirroringWorld.h"
#include "AL/alut.h"

extern SDL_Surface* screen;
extern char* ipaddy;
extern int WIDTH;
extern int HEIGHT;

class GameViewController : public HBViewController {
	private:
		void _disconnect();
		void _initMenus();
		void _initSound();

	protected:
		unsigned int albuf[3], alsrcs[ALSRCS];
		Socket *sock;
		SocketConnection* sc;
		MirroringWorld world;
		float angle;
		int myId;
		int latestPacket;
		int count;
		int oldTime;
		menu* mainmenu;

		ALfloat pos[3], vel[3], ori[6];

	public:
		GameViewController();
		~GameViewController();
		HBViewMode didFinishView();
		void process();
		void render() {}
		bool quit();
		bool leave();
};

class leavefunc {
	public:
		GameViewController* _gvc;
		leavefunc() {}
		leavefunc(GameViewController* gvc) {
			_gvc = gvc;
		}
		bool operator()(voidtype) {
			return _gvc->leave();
		}
};

#endif
