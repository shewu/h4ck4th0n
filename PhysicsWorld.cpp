#include "PhysicsWorld.h"

using std::map;
using std::pair;
using std::vector;

FlagObject* PhysicsWorld::addFlagObject(
		int teamNumber, vector<SpawnDescriptor> *possibleSpawns) {
	FlagObject *obj = new FlagObject(teamNumber, SPAWN_TIME);
	movingRoundObjects.insert(pair<int, MovingRoundObject*>(obj->getID(), obj));
}

PlayerObject* PhysicsWorld::addPlayerObject(
		int teamNumber, vector<SpawnDescriptor> *possibleSpawns) {
	PlayerObject *obj = new PlayerObject(teamNumber, SPAWN_TIME);
	movingRoundObjects.insert(pair<int, MovingRoundObject*>(obj->getID(), obj));
}

void PhysicsWorld::removeDeadObjects() {
	auto iter = movingRoundObjects.begin();
	while(iter != movingRoundObjects.end()) {
		auto iter2 = iter;
		++iter2;

		MovingRoundObject *obj = iter->second;
		if (obj->getState() == MOS_DEAD && obj->getRefCount() == 0) {
			movingRoundObjects.erase(iter);
			delete obj;
		}

		iter = iter2;
	}
}

void PhysicsWorld::writeToPacket(WritePacket *wp) const {
	for(auto iter = movingRoundObjects.begin(); iter != movingRoundObjects.end(); ++iter) {
		MovingRoundObject *obj = iter->second;
		wp->write_int(obj->getID());
		obj->writeToPacket(wp);
	}
}
