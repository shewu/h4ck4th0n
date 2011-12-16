#include <AL/alut.h>
#include <AL/al.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "GameViewController.h"
#include "hack.h"
#include "menu.h"

extern SDL_Surface* screen;
extern char* ipaddy;
extern int WIDTH;
extern int HEIGHT;
extern bool NORAPE;

class UnholyGameViewController : public GameViewController {
	private:
		unsigned int albuf[3], alsrcs[ALSRCS];
		SocketConnection* sc;
		World world;
		float angle;
		int myId;
		menu* mainmenu;
		GLUquadric* quad;
		void _initGL();
		void _initMenus();
		void _initSound();
		void _drawWalls();
		void _drawObjects();
		void _drawFloor(float);
		void _disconnect();
		int program;

	public:
		UnholyGameViewController();
		~UnholyGameViewController();

		HBViewMode didFinishView();
		void process();
		void render();
		bool leave();
		bool quit();
};

