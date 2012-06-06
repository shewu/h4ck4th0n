#include "World.h"

class PhysicsWorld : World
{
	public:
		void doSimulation(float dt);

		FlagObject *addFlagObject(int teamNumber, vector<SpawnDescriptor> *possibleSpawns);
		PlayerObject *addPlayerObject(int teamNumber, vector<SpawnDescriptor> *possibleSpawns);

		void removeDeadObjects();

		void writeUpdates(WritePacket *wp);

	private:
		vector<pair<Object*,ObjectType> > additions;
		vector<int> deletions;
};
