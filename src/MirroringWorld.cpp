#include "MirroringWorld.h"

void MirroringWorld::readFromPacket(ReadPacket *rp) {
    int n;

    n = rp->read_int();
    movingRoundObjects.clear();
    for (int i = 0; i < n; i++) {
        MovingRoundObject *obj = new MovingRoundObject(rp);
        movingRoundObjects[obj->getID()] = obj;
    }

    n = rp->read_int();
    rectangularWalls.clear();
    for (int i = 0; i < n; i++) {
        RectangularWall *obj = new RectangularWall(rp);
        rectangularWalls[obj->getID()] = obj;
    }

    n = rp->read_int();
    roundWalls.clear();
    for (int i = 0; i < n; i++) {
        RoundWall *obj = new RoundWall(rp);
        roundWalls[obj->getID()] = obj;
    }

    myId_ = rp->read_int();
}

MovingRoundObject *MirroringWorld::getMyObject() {
    if (myId_ == -1) {
        return NULL;
    } else {
        return movingRoundObjects[myId_];
    }
}
