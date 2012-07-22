#include "PhysicsWorld.h"

void
PhysicsWorld::applyForces(float dt) {
	for (auto& iter : movingRoundObjects) {
		MovingRoundObject& obj = *(iter.second);
		if (obj.state == MOS_ALIVE) {
			obj.velocity -= obj.velocity * FRICTION * dt;
		}
	}
}
