#include <AL/alut.h>
#include <AL/al.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#ifndef UNHOLY
#include <CL/cl.hpp>
#endif

#include "GameViewController.h"
#include "hack.h"
#include "menu.h"

extern SDL_Surface* screen;
extern char* ipaddy;
extern int WIDTH;
extern int HEIGHT;

class MultiGameViewController : public GameViewController {
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
		vector<cl::Context> deviceContexts;
		vector<cl::Program> devicePrograms;
		vector<cl::Device> devices;
		vector<cl::Image2D> images;
		vector<cl::CommandQueue> cqs;
		GLuint texture;
		int* pixels;

	public:
		MultiGameViewController();
		~MultiGameViewController();

		HBViewMode didFinishView();
		void process();
		void render();
		bool leave();
		bool quit();
};

