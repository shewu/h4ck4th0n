#ifndef __GAMEVIEWCONTROLLER_H__
#define __GAMEVIEWCONTROLLER_H__

#include "HBViewController.h"
#include "hack.h"
#include "menu.h"
#include "menufuncs.h"

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
		SocketConnection* sc;
		World world;
		float angle;
		int myId;
		menu* mainmenu;

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
	private:
		GameViewController* _gvc;

	public:
		leavefunc() {}
		leavefunc(GameViewController* gvc) {
			_gvc = gvc;
		}
		bool operator()(voidtype) {
			return _gvc->leave();
		}
};

#endif

