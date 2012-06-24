#include "World.h"

#include <vector>

class PhysicsWorld : World
{
	public:
		void doSimulation(float dt);

		FlagObject *addFlagObject(int teamNumber,
		    std::vector<SpawnDescriptor> *possibleSpawns);

		PlayerObject *addPlayerObject(int teamNumber,
		    std::vector<SpawnDescriptor> *possibleSpawns);

		void removeDeadObjects();

		void writeToPacket(WritePacket *wp) const;

	private:
};
