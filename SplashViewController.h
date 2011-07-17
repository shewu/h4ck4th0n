#include "menu.h"
#include "font.h"
#include "HBViewController.h"

class SplashViewController : public HBViewController {
	private:
		menu* splashMenu;
		bool finishedView;

	public:
		SplashViewController();
		~SplashViewController();

		bool menuFinishedView();
		bool didFinishView();
		void process();
		void render();
};


