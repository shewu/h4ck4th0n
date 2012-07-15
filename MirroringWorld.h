#ifndef MIRRORING_WORLD_H
#define MIRRORING_WORLD_H

#include "World.h"

class MirroringWorld : public World
{
	public:
		MirroringWorld() : World() { }
		MirroringWorld(HBMap const& hbmap) : World(hbmap) { }

		void readFromPacket(ReadPacket *rp);
};

#endif
