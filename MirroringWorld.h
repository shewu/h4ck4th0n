#ifndef MIRRORING_WORLD_H
#define MIRRORING_WORLD_H

class MirroringWorld : public World
{
	public:
		void readFromPacket(ReadPacket *rp);
};

#endif
