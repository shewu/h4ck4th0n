#include "PhysicsWorld.h"

using std::map;
using std::pair;
using std::vector;

ObjectPtr<MovingRoundObject> PhysicsWorld::addFlagObject(
	int teamNumber,
	std::function<void()> const& onSpawnCallback) {
	MovingRoundObject *obj = new MovingRoundObject(
			Object::materialsByTeamNumber[teamNumber],
			MovingRoundObject::kFlagRadius,
			MovingRoundObject::kFlagMass,
			MovingRoundObject::kFlagHeightRatio,
			SPAWN_TIME,
			onSpawnCallback,
			teamNumber,
			true);
	movingRoundObjects.insert(pair<int, MovingRoundObject*>(obj->getID(), obj));
	return ObjectPtr<MovingRoundObject>(obj);
}

ObjectPtr<MovingRoundObject> PhysicsWorld::addPlayerObject(
	int teamNumber,
	std::function<void()> const& onSpawnCallback) {
	MovingRoundObject *obj = new MovingRoundObject(
			Object::materialsByTeamNumber[teamNumber],
			MovingRoundObject::kPlayerRadius,
			MovingRoundObject::kPlayerMass,
			MovingRoundObject::kPlayerHeightRatio,
			SPAWN_TIME,
			onSpawnCallback,
			teamNumber,
			false);
	movingRoundObjects.insert(pair<int, MovingRoundObject*>(obj->getID(), obj));
	return ObjectPtr<MovingRoundObject>(obj);
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
