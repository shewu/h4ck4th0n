#include "menu.h"
#include "font.h"
#include "HBViewController.h"

class ServerConnectViewController : public HBViewController {
	private:
		menu* serverConnectMenu;

	public:
		ServerConnectViewController();
		~ServerConnectViewController();

		bool didFinishView();
		void process();
		void render();
};


