#ifndef MENU_H
#define MENU_H

#include "Menu.h"
#include "Font.h"
#include "HBViewController.h"

class SplashViewController : public HBViewController {
	private:
		menu* splashMenu;

	public:
		SplashViewController();
		~SplashViewController();

		HBViewMode didFinishView();
		void process();
		void render();
		bool quit();
};

#endif
