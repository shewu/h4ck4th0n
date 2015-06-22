#ifndef MIRRORING_WORLD_H
#define MIRRORING_WORLD_H

#include "World.h"

class MirroringWorld : public World {
public:
    MirroringWorld() : World(), myId_(-1) {}
    MirroringWorld(HBMap const &hbmap) : World(hbmap), myId_(-1) {}

    void readFromPacket(ReadPacket *rp);

    MovingRoundObject *getMyObject();

private:
    int myId_;
};

#endif
