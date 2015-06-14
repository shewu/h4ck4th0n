#ifndef SPLASH_VIEW_CONTROLLER_H
#define SPLASH_VIEW_CONTROLLER_H

#include "Menu.h"
#include "Font.h"
#include "HBViewController.h"

class SplashViewController : public HBViewController {
	private:
		Menu* splashMenu;

	public:
		SplashViewController();
		~SplashViewController();

		HBViewMode didFinishView();
		void process();
		void render();
		bool quit();
};

#endif
