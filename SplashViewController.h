#include "menu.h"
#include "font.h"
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


