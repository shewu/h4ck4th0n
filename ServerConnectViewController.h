#include "menu.h"
#include "font.h"
#include "HBViewController.h"

class ServerConnectViewConroller : public HBViewController {
	private:
		menu* serverConnectMenu;
		bool finishedView;
	public:
		bool didFinishView();
		ServerConnectViewConroller();
		~ServerConnectViewConroller();
};


