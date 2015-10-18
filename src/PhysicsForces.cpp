#include "PhysicsWorld.h"

#include "Constants.h"

void PhysicsWorld::applyForces(float dt) {
    for (auto &iter : movingRoundObjects) {
        MovingRoundObject &obj = *(iter.second);

        float frictionCoeff = FRICTION;
        for (auto const &floor : worldMap.getFloors()) {
            float newFrictionCoeff;
            if (floor.getFrictionCoeff(obj.center, newFrictionCoeff)) {
                frictionCoeff = newFrictionCoeff;
                break;
            }
        }

        // Apply force from friction
        if (obj.state == MOS_ALIVE) {
            obj.velocity -= obj.velocity * frictionCoeff * dt;
        }

        // Apply force from swallowing
        MovingRoundObject *swallower = obj.getSwallowerParent();
        if (swallower != NULL) {
            MovingRoundObject &obj2 = *swallower;

            Vector2D offset = obj2.center - obj.center;
            Vector2D force = SWALLOWING_SPRING_CONSTANT * offset;
            obj.velocity += force * dt;
            obj2.velocity -= force * dt;
        }
    }
}
