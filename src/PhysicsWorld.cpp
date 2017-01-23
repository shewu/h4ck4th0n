#include "PhysicsWorld.h"

#include "Constants.h"

using std::map;
using std::pair;
using std::vector;

std::shared_ptr<MovingRoundObject> PhysicsWorld::addFlagObject(
    int regionNumber, std::function<void()> const &onSpawnCallback,
    std::function<void()> const &onDeathCallback) {
    std::shared_ptr<MovingRoundObject> obj(new MovingRoundObject(
        Object::materialsByTeamNumber[regionNumber],  // TODO fix this, this
                                                      // will
        // work for now but materials
        // should not in general come
        // from here (honestly, we
        // should probably remove
        // this function completely)
        MovingRoundObject::kFlagRadius, MovingRoundObject::kFlagMass,
        MovingRoundObject::kFlagHeightRatio, SPAWN_TIME, regionNumber,
        onSpawnCallback, onDeathCallback));
    movingRoundObjects.insert(std::make_pair(obj->getID(), obj));
    return obj;
}

std::shared_ptr<MovingRoundObject> PhysicsWorld::addPlayerObject(
    int regionNumber, std::function<void()> const &onSpawnCallback,
    std::function<void()> const &onDeathCallback) {
    std::shared_ptr<MovingRoundObject> obj(new MovingRoundObject(
        Object::materialsByTeamNumber[regionNumber],
        MovingRoundObject::kPlayerRadius, MovingRoundObject::kPlayerMass,
        MovingRoundObject::kPlayerHeightRatio, SPAWN_TIME, regionNumber,
        onSpawnCallback, onDeathCallback));
    movingRoundObjects.insert(std::make_pair(obj->getID(), obj));
    return obj;
}

void PhysicsWorld::removeDeadObjects() {
    auto iter = movingRoundObjects.begin();
    while (iter != movingRoundObjects.end()) {
        auto iter2 = iter;
        ++iter2;

        std::shared_ptr<MovingRoundObject> const& objPtr = iter->second;
        if (objPtr->getState() == MOS_DEAD && objPtr.use_count() == 1) {
            movingRoundObjects.erase(iter);
        }

        iter = iter2;
    }
}

void PhysicsWorld::writeToPacket(WritePacket *wp) const {
    int size = 0;
    for (auto &pa : movingRoundObjects) {
        auto obj = pa.second;
        if (obj->state == MOS_ALIVE || obj->state == MOS_SHRINKING) {
            size++;
        }
    }
    wp->write_int(size);
    for (auto &pa : movingRoundObjects) {
        auto obj = pa.second;
        if (obj->state == MOS_ALIVE || obj->state == MOS_SHRINKING) {
            obj->writeToPacket(wp);
        }
    }

    wp->write_int(static_cast<int>(rectangularWalls.size()));
    for (auto &pa : rectangularWalls) {
        auto obj = pa.second;
        obj->writeToPacket(wp);
    }

    wp->write_int(static_cast<int>(roundWalls.size()));
    for (auto &pa : roundWalls) {
        auto obj = pa.second;
        obj->writeToPacket(wp);
    }
}
