#ifndef __GAMEVIEWCONTROLLER_H__
#define __GAMEVIEWCONTROLLER_H__

#include "HBViewController.h"

class GameViewController : public HBViewController {
	public:
		HBViewMode didFinishView() { return (HBViewMode)0; }
		void process() {}
		void render() {}
		bool quit() { return true; }
		bool leave() { return true; }
};

#endif

