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
}
