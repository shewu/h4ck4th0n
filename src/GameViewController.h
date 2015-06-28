#ifndef __GAMEVIEWCONTROLLER_H__
#define __GAMEVIEWCONTROLLER_H__

#include <AL/alut.h>
#ifndef __APPLE__
#include <AL/al.h>
#include <GL/gl.h>
#else
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenGL/glu.h>
#endif
#include <SDL2/SDL.h>

#include "HBViewController.h"

#include <memory>

#include "Menu.h"
#include "MirroringWorld.h"
#include "Packet.h"
#include "Socket.h"
#include "Sound.h"

extern std::string ipaddy;
extern int WIDTH;
extern int HEIGHT;

class GameViewController : public HBViewController {
public:
    GameViewController();
    ~GameViewController();
    HBViewMode didFinishView();
    void process();
    bool quit();
    bool leave();

protected:
    unsigned int albuf[NUM_SOUND_TYPES], alsrcs[ALSRCS];
    Socket *sock;
    SocketConnection *sc;
    std::unique_ptr<MirroringWorld> world;
    float angle;
    int latestPacket;
    int count;
    int oldTime;
    Menu *mainmenu;
    Menu *_resmenu;

    ALfloat pos[3], vel[3], ori[6];

    virtual void _initGL() = 0;

private:
    void _disconnect();
    void _initMenus();
    void _initSound();
};

#endif
