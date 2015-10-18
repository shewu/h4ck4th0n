#include "PhysicsWorld.h"

#include "Constants.h"

using std::map;
using std::pair;
using std::vector;

ObjectPtr<MovingRoundObject> PhysicsWorld::addFlagObject(
    int regionNumber, std::function<void()> const &onSpawnCallback,
    std::function<void()> const &onDeathCallback) {
    MovingRoundObject *obj = new MovingRoundObject(
        Object::materialsByTeamNumber[regionNumber],  // TODO fix this, this
                                                      // will
        // work for now but materials
        // should not in general come
        // from here (honestly, we
        // should probably remove
        // this function completely)
        MovingRoundObject::kFlagRadius, MovingRoundObject::kFlagMass,
        MovingRoundObject::kFlagHeightRatio, SPAWN_TIME, regionNumber,
        onSpawnCallback, onDeathCallback);
    movingRoundObjects.insert(
        pair<int, MovingRoundObject *>(obj->getID(), obj));
    return ObjectPtr<MovingRoundObject>(obj);
}

ObjectPtr<MovingRoundObject> PhysicsWorld::addPlayerObject(
    int regionNumber, std::function<void()> const &onSpawnCallback,
    std::function<void()> const &onDeathCallback) {
    MovingRoundObject *obj = new MovingRoundObject(
        Object::materialsByTeamNumber[regionNumber],
        MovingRoundObject::kPlayerRadius, MovingRoundObject::kPlayerMass,
        MovingRoundObject::kPlayerHeightRatio, SPAWN_TIME, regionNumber,
        onSpawnCallback, onDeathCallback);
    movingRoundObjects.insert(
        pair<int, MovingRoundObject *>(obj->getID(), obj));
    return ObjectPtr<MovingRoundObject>(obj);
}

void PhysicsWorld::removeDeadObjects() {
    auto iter = movingRoundObjects.begin();
    while (iter != movingRoundObjects.end()) {
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
    int size = 0;
    for (auto &pa : movingRoundObjects) {
        MovingRoundObject *obj = pa.second;
        if (obj->state == MOS_ALIVE || obj->state == MOS_SHRINKING || obj->state == MOS_BEING_SWALLOWED) {
            size++;
        }
    }
    wp->write_int(size);
    for (auto &pa : movingRoundObjects) {
        MovingRoundObject *obj = pa.second;
        if (obj->state == MOS_ALIVE || obj->state == MOS_SHRINKING || obj->state == MOS_BEING_SWALLOWED) {
            obj->writeToPacket(wp);
        }
    }

    wp->write_int(static_cast<int>(rectangularWalls.size()));
    for (auto &pa : rectangularWalls) {
        RectangularWall *obj = pa.second;
        obj->writeToPacket(wp);
    }

    wp->write_int(static_cast<int>(roundWalls.size()));
    for (auto &pa : roundWalls) {
        RoundWall *obj = pa.second;
        obj->writeToPacket(wp);
    }
}
