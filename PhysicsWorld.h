#include "World.h"

class PhysicsWorld : World
{
	public:
		void doSimulation(float dt);

		FlagObject *addFlagObject(int teamNumber, vector<SpawnDescriptor> *possibleSpawns);
		PlayerObject *addPlayerObject(int teamNumber, vector<SpawnDescriptor> *possibleSpawns);

		void removeDeadObjects();

		void writeToPacket(WritePacket *wp);

	private:
};
