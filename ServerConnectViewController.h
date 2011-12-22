#include "menu.h"
#include "font.h"
#include "HBViewController.h"

class ServerConnectViewController : public HBViewController {
	private:
		menu* serverConnectMenu;

	public:
		ServerConnectViewController();
		~ServerConnectViewController();

		HBViewMode didFinishView();
		void process();
		void render();
		bool quit();
};


