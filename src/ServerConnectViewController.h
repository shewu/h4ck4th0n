#ifndef SERVER_CONNECT_VIEW_CONTROLLER_H
#define SERVER_CONNECT_VIEW_CONTROLLER_H

#include "Menu.h"
#include "Font.h"
#include "HBViewController.h"

class ServerConnectViewController : public HBViewController {
private:
    Menu *serverConnectMenu;

public:
    ServerConnectViewController();
    ~ServerConnectViewController();

    HBViewMode didFinishView();
    void process();
    void render();
    bool quit();
};

#endif
