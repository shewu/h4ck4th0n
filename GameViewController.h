#ifndef __GAMEVIEWCONTROLLER_H__
#define __GAMEVIEWCONTROLLER_H__

#ifndef __APPLE__
#include <AL/alut.h>
#include <AL/al.h>
#include <GL/gl.h>
#else
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenGL/glu.h>
#endif

#include "HBViewController.h"
#include "hack.h"
#include "menu.h"
#include "packet.h"

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
		World world;
		float angle;
		int myId;
		long latestPacket;
		int count;
		int oldTime;
		menu* mainmenu;

		ALfloat pos[3], vel[3], ori[6];
    
        virtual void _initGL() = 0;

	public:
		GameViewController();
		~GameViewController();
		HBViewMode didFinishView();
		void process();
		bool quit();
		bool leave();
        bool change_resolution (int width, int height);
};

class leavefunc {
public:
	GameViewController* _gvc;
    leavefunc() : _gvc(NULL) {}
    leavefunc(GameViewController* gvc) : _gvc(gvc) {}
	bool operator()(voidtype) {
        if (_gvc) {
            return _gvc->leave();
        }
        return false;
	}
};

class change_resolution_func {
public:
    change_resolution_func (GameViewController *gvc, int width, int height) : _gvc(gvc), _width(width), _height(height) {}
    bool operator()(voidtype) {
        SDL_SetWindowSize(screen, _width, _height);
        return _gvc->change_resolution(_width, _height);
    }
    
private:
    GameViewController *_gvc;
    const int _width, _height;
};

#endif

