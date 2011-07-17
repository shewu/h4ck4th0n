#include "menu.h"
#include "font.h"
#include "HBViewController.h"

class SplashViewController : public HBViewController {
	private:
		menu* splashMenu;
		bool finishedView;
	public:
		bool didFinishView();
		SplashViewController();
		~SplashViewController();
};


