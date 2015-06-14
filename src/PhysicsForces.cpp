#include "PhysicsWorld.h"

#include "Constants.h"

void
PhysicsWorld::applyForces(float dt) {
	for (auto& iter : movingRoundObjects) {
		MovingRoundObject& obj = *(iter.second);

		float frictionCoeff = FRICTION;
		for (auto const& floor : worldMap.getFloors()) {
			float newFrictionCoeff;
			if (floor.getFrictionCoeff(obj.center, newFrictionCoeff)) {
				frictionCoeff = newFrictionCoeff;
				break;
			}
		}

		if (obj.state == MOS_ALIVE) {
			obj.velocity -= obj.velocity * frictionCoeff * dt;
		}
	}
}
