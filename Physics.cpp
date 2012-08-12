#include <cmath>
#include <map>

#include "Constants.h"
#include "Object.h"
#include "PhysicsWorld.h"
#include "Util.h"

using std::function;
using std::map;
using std::pair;
using std::priority_queue;
using std::vector;

// geometry helper methods
namespace  {
	void intersectRayWithCircle(Vector2D p, Vector2D v, Vector2D center, float radius,
	                            float& time1, float& theta1, float& time2, float& theta2) {
		float a = v.x * v.x + v.y * v.y;
		if (a == 0.0) {
			time1 = INFINITY;
			time2 = INFINITY;
			theta1 = 0.0;
			theta2 = 0.0;
			return;
		}
		float b = 2.0 * (v.x * (p.x - center.x) + v.y * (p.y - center.y));
		float c = (p.x - center.x) * (p.x - center.x) +
		          (p.y - center.y) * (p.y - center.y) - radius * radius;
		float d = b*b - 4.0f*a*c;
		if (d < 0.0) {
			time1 = INFINITY;
			time2 = INFINITY;
			theta1 = 0.0;
			theta2 = 0.0;
			return;
		}
		d = sqrt(d);
		time1 = (-b - d) / (2.0 * a);
		time2 = (-b + d) / (2.0 * a);

		if (time1 < 0.0) {
			time1 = INFINITY;
			theta1 = 0.0;
		} else {
			theta1 = (p + time1*v - center).getAngle();
		}

		if (time2 < 0.0) {
			time2 = INFINITY;
			theta2 = 0.0;
		} else {
			theta2 = (p + time2*v - center).getAngle();
		}
	}
	
	bool isAngleInRange(float theta1, float theta2, float theta) {
		float thetap = theta - TWO_PI;
		return (theta1 <= theta  && theta  <= theta2) ||
			   (theta1 <= thetap && thetap <= theta2);
	}
} // anonymous namespace

bool PhysicsWorld::objectsIntersect(MovingRoundObject const& obj1, MovingRoundObject const& obj2) {
	return ((obj2.state == MOS_ALIVE || obj2.state == MOS_SHRINKING) &&
	        (obj2.center.x - obj1.center.x) * (obj2.center.x - obj1.center.x) +
	        (obj2.center.y - obj1.center.y) * (obj2.center.y - obj1.center.y) <=
	        (obj1.radius + obj2.radius) * (obj1.radius + obj2.radius));
}

bool PhysicsWorld::objectsIntersect(MovingRoundObject const& obj, RectangularWall const& wall) {
	if ((obj.center - wall.p1).lengthSquared() <= obj.radius * obj.radius ||
	    (obj.center - wall.p2).lengthSquared() <= obj.radius * obj.radius) {
		return true;
	}

	Vector2D dir = wall.p2 - wall.p1;
	if ((dir * wall.p1) < (dir * obj.center) &&
	    (dir * wall.p2) > (dir * obj.center)) {
		Vector2D vec = obj.center - wall.p1;
		Vector2D proj = ((vec*dir) / dir.lengthSquared()) * dir;
		return (vec - proj).lengthSquared() <= obj.radius * obj.radius;
	}

	return false;
}

/**
 * Computes the time when two circle objects will intersect.
 * @param diff Center of B relative to A.
 * @param vel Velocity of B relative to A.
 * @param r Sum of radii of A and B.
 * @param rc Total rate at which r is decreasing.
 * @return The time at which the two objects will intersect if uninterrupted.
 */
float PhysicsWorld::collideCircles(Vector2D diff, Vector2D vel, float r, float rc) {
	if (diff*vel+rc*sqrt(diff*diff) >= 0) return INFINITY;
	float a = vel*vel-rc*rc, b = 2*(diff*vel)+2*rc*r, c = diff*diff-r*r;
	float discrim = b*b-4*a*c;
	if (discrim <= 0) return INFINITY;
	float ans = (-b-sqrt(discrim))/(2*a);
	if (ans <= 0) return 0;
	return ans;
}

/**
 * Does object collision for the two given MovingRoundObjects.
 * @param fo One of the objects for the collision.
 * @param so The second of the objects for the collision.
 * @param collideRoundWithRound A map to the event of the collision, with key
                            (fo.getID(), so.getID()).
 * @param collideEvents A queue of events to add an event to if there is one.
 * @param cur The current time.
 * @param dt The amount of time to look into the future. Collision is not added to the queue
 *           if it occurs after dt time has passed.
 */
void PhysicsWorld::doObjectCollision(MovingRoundObject const& fo, MovingRoundObject const& so,
                       map<pair<int,int>, PhysicsWorld::collide_event>& collideRoundWithRound,
                       priority_queue<PhysicsWorld::collide_event>& collideEvents,
                       float cur, float dt) {
	if (fo.state == MOS_DEAD || so.state == MOS_DEAD ||
		fo.state == MOS_SPAWNING || so.state == MOS_SPAWNING || 
		(fo.state == MOS_SHRINKING && so.state == MOS_SHRINKING)) {
		if (fo.getID() < so.getID()) {
			collideRoundWithRound[pair<int, int>(fo.getID(), so.getID())] =
						   collide_event(INFINITY, ET_NONE, fo.getID(), so.getID());
		} else {
			collideRoundWithRound[pair<int, int>(so.getID(), fo.getID())] =
						   collide_event(INFINITY, ET_NONE, so.getID(), fo.getID());
		}
		return;
	}

	float rc = 0.0;
	Vector2D v(0, 0);
	if (fo.state == MOS_ALIVE || fo.isCurrentlyShrinking()) {
		v -= fo.velocity;
	}
	if (fo.isCurrentlyShrinking()) {
		rc += DEATH_RATE;
	}
	if (so.state == MOS_ALIVE || fo.isCurrentlyShrinking()) {
		v += so.velocity;
	}
	if (so.isCurrentlyShrinking()) {
		rc += DEATH_RATE;
	}
	
	float t = collideCircles(so.center - fo.center, v,
	                         so.radius + fo.radius, rc) + cur;
	if (fo.getID() < so.getID()) {
		collide_event e(t, ET_ROUND_ROUND, fo.getID(), so.getID());
		collideRoundWithRound[pair<int, int>(fo.getID(), so.getID())] = e;
		if (t < dt) {
			collideEvents.push(e);
		}
	} else {
		collide_event e(t, ET_ROUND_ROUND, so.getID(), fo.getID());
		collideRoundWithRound[pair<int, int>(so.getID(), fo.getID())] = e;
		if (t < dt) {
			collideEvents.push(e);
		}
	}
}

void PhysicsWorld::doRectangularWallCollision(MovingRoundObject const& obj,
                                RectangularWall const& wall,
                                map<pair<int, int>, PhysicsWorld::collide_event>& collideRoundWithWall,
                                priority_queue<PhysicsWorld::collide_event>& collideEvents,
                                float cur, float dt) {
    if (obj.state != MOS_ALIVE) {
		collideRoundWithWall[pair<int, int>(obj.getID(), wall.getID())] =
		        collide_event(INFINITY, ET_NONE, obj.getID(), wall.getID());
		return;
	}

	float time1 = INFINITY;
	float time2 = INFINITY;
	if ((wall.p1 - obj.center) * obj.velocity > 0.0) {
		time1 = collideCircles(obj.center - wall.p1, obj.velocity,
									 obj.radius, 0);
	}
	if ((wall.p2 - obj.center) * obj.velocity > 0.0) {
		time2 = collideCircles(obj.center - wall.p2, obj.velocity,
									 obj.radius, 0);
	}

	float time3;
	Vector2D dir = wall.p2 - wall.p1;
	Vector2D relpos = obj.center - wall.p1;
	Vector2D normal = dir.getNormalVector();
	float dist = relpos * normal;
	if (dist < 0) {
		normal = -normal;
		dist = -dist;
	}
	if (obj.velocity * normal >= 0) {
		time3 = INFINITY;
	} else {
		time3 = (obj.radius - dist) / (obj.velocity * normal);
		if (time3 < 0) {
			time3 = 0;
		}
		Vector2D newpos = relpos + time3 * obj.velocity;
		if (newpos * dir < 0 || newpos * dir > dir*dir) {
			time3 = INFINITY;
		}
	}

	EventType type;
	float mintime;
	if (time1 < time2 && time1 < time3) {
		mintime = time1;
		type = ET_ROUND_WALL_CORNER_1;
	} else if (time2 < time3) {
		mintime = time2;
		type = ET_ROUND_WALL_CORNER_2;
	} else {
		mintime = time3;
		type = ET_ROUND_WALL_LINE;
	}
	float t = cur + mintime;
	collide_event e(t, type, obj.getID(), wall.getID());
	collideRoundWithWall[pair<int, int>(obj.getID(), wall.getID())] = e;
	if (t < dt) {
		collideEvents.push(e);
	}
}

void PhysicsWorld::doRoundWallCollision(MovingRoundObject const& obj,
                                RoundWall const& wall,
                                map<pair<int, int>, PhysicsWorld::collide_event>& collideRoundWithRoundWall,
                                priority_queue<PhysicsWorld::collide_event>& collideEvents,
                                float cur, float dt) {
	if (obj.state != MOS_ALIVE) {
		collideRoundWithRoundWall[pair<int, int>(obj.getID(), wall.getID())] =
		        collide_event(INFINITY, ET_NONE, obj.getID(), wall.getID());
		return;
	}

	Vector2D p1 = wall.center + wall.radius * Vector2D(cosf(wall.theta1), sinf(wall.theta1));
	Vector2D p2 = wall.center + wall.radius * Vector2D(cosf(wall.theta2), sinf(wall.theta2));
	float time1 = INFINITY;
	float time2 = INFINITY;
	if ((p1 - obj.center) * obj.velocity > 0.0) {
		time1 = collideCircles(obj.center - p1, obj.velocity,
									 obj.radius, 0);
	}
	if ((p2 - obj.center) * obj.velocity > 0.0) {
		time2 = collideCircles(obj.center - p2, obj.velocity,
									 obj.radius, 0);
	}

	float time3 = INFINITY;

	float t1, t2, theta1, theta2;

	if ((obj.center - wall.center).lengthSquared() > wall.radius * wall.radius && obj.velocity * (wall.center - obj.center) > 0) {
		intersectRayWithCircle(obj.center, obj.velocity, wall.center, wall.radius + obj.radius, t1, theta1, t2, theta2);
		if (t1 < time3 && isAngleInRange(wall.theta1, wall.theta2, theta1)) {
			time3 = t1;
		}
		if (t2 < time3 && isAngleInRange(wall.theta1, wall.theta2, theta2)) {
			time3 = t2;
		}
	} else if ((obj.center - wall.center).lengthSquared() < wall.radius * wall.radius && obj.velocity * (wall.center - obj.center) < 0) {
		intersectRayWithCircle(obj.center, obj.velocity, wall.center, wall.radius - obj.radius, t1, theta1, t2, theta2);
		if (t1 < time3 && isAngleInRange(wall.theta1, wall.theta2, theta1)) {
			time3 = t1;
		}
		if (t2 < time3 && isAngleInRange(wall.theta1, wall.theta2, theta2)) {
			time3 = t2;
		}
	}

	EventType type;
	float mintime;
	if (time1 < time2 && time1 < time3) {
		mintime = time1;
		type = ET_ROUND_ROUNDWALL_CORNER_1;
	} else if (time2 < time3) {
		mintime = time2;
		type = ET_ROUND_ROUNDWALL_CORNER_2;
	} else {
		mintime = time3;
		type = ET_ROUND_ROUNDWALL_LINE;
	}

	float t = cur + mintime;
	collide_event e(t, type, obj.getID(), wall.getID());
	collideRoundWithRoundWall[pair<int, int>(obj.getID(), wall.getID())] = e;
	if (t < dt) {
		collideEvents.push(e);
	}
}

void PhysicsWorld::doRoundObjectDisappearing(MovingRoundObject const& obj,
		map<int, PhysicsWorld::collide_event>& collideDisappear,
		priority_queue<PhysicsWorld::collide_event>& collideEvents,
		float cur, float dt) {
	if (obj.isCurrentlyShrinking()) {
		float time = obj.radius / DEATH_RATE;
		collide_event e(cur + time, ET_ROUND_DISAPPEAR, obj.getID(), -1);
		collideDisappear[obj.getID()] = e;
		if (time < dt) {
			collideEvents.push(e);
		}
	} else {
		collideDisappear[obj.getID()] = collide_event(INFINITY, ET_NONE, obj.getID(), -1);
	}
}

/**
 * What should happen if a obj2 is moving and hits obj1. Starts obj2 shrinking if necessary;
 * otherwise, updates its velocity.
 * @param obj1 A shrinking round object.
 * @param obj2 A moving object which hits obj1.
 */
void PhysicsWorld::bounceMovingRoundAndShrinkingRound(
		MovingRoundObject& obj1,
		MovingRoundObject& obj2, bool shouldDie) {
	Vector2D normal = obj2.center - obj1.center;
	float nv1 = obj1.velocity * normal;
	float nv2 = obj2.velocity * normal;
	if (shouldDie) {
		obj2.startShrinking(&obj1, -normal*(1/sqrt(normal*normal))*DEATH_RATE);
	} else {
		if (obj1.isCurrentlyShrinking()) {
			obj2.velocity -= 2.0 * ((nv2-nv1)/(normal*normal)+(1/sqrt(normal*normal))*DEATH_RATE)*normal;
		} else {
			obj2.velocity -= 2.0*(nv2 / (normal*normal)) * normal;
		}
	}
}

void PhysicsWorld::bounceMovingRoundObjectByNormal(MovingRoundObject& obj, Vector2D const& normal, float bouncinessCoefficient) {
	float nv1 = obj.velocity * normal;
	obj.velocity -= (1.0 + bouncinessCoefficient) * (nv1/(normal*normal)) * normal;
}

void PhysicsWorld::updateRoundObjectsForward(map<int, MovingRoundObject*>& objects, float dt) {
	for (auto& pa : objects) {
		MovingRoundObject& obj = *(pa.second);
		if (obj.state == MOS_ALIVE) {
			obj.center += obj.velocity * dt;
		}
		else if (obj.isCurrentlyShrinking()) {
			obj.center += obj.velocity * dt;
			obj.radius -= DEATH_RATE * dt;
		}
	}
}

void PhysicsWorld::doSimulation(float dt) {
	map<pair<int, int>, PhysicsWorld::collide_event> collideRoundWithRound;
	map<pair<int, int>, PhysicsWorld::collide_event> collideRoundWithWall;
	map<pair<int, int>, PhysicsWorld::collide_event> collideRoundWithRoundWall;
	map<int, PhysicsWorld::collide_event> collideDisappear;
	priority_queue<PhysicsWorld::collide_event> collideEvents;

	for (auto i = movingRoundObjects.begin(); i != movingRoundObjects.end(); ++i) {
		auto j = i;
		for (++j; j != movingRoundObjects.end(); ++j) {
			doObjectCollision(*(i->second), *(j->second), collideRoundWithRound, collideEvents, 0, dt);
		}
		for(auto const& pa : rectangularWalls) {
			doRectangularWallCollision(*(i->second), *(pa.second), collideRoundWithWall, collideEvents, 0, dt);
		}
		for(auto const& pa : roundWalls) {
			doRoundWallCollision(*(i->second), *(pa.second), collideRoundWithRoundWall, collideEvents, 0, dt);
		}
		doRoundObjectDisappearing(*(i->second), collideDisappear, collideEvents, 0, dt);
	}

	int eventsDone = 0;
	float knownTime = 0.0;
	while (!collideEvents.empty() && collideEvents.top().time < dt && eventsDone < MAX_EVENTS) {
		collide_event e = collideEvents.top();
		collideEvents.pop();

		if ((e.type == ET_ROUND_ROUND     && collideRoundWithRound[pair<int, int>(e.t1, e.t2)] != e) ||
				(e.type == ET_ROUND_DISAPPEAR && collideDisappear[e.t1] != e) ||
				((e.type == ET_ROUND_WALL_CORNER_1 || e.type == ET_ROUND_WALL_CORNER_2 || e.type == ET_ROUND_WALL_LINE) && collideRoundWithWall[pair<int, int>(e.t1, e.t2)] != e) ||
				((e.type == ET_ROUND_ROUNDWALL_CORNER_1 || e.type == ET_ROUND_ROUNDWALL_CORNER_2 || e.type == ET_ROUND_ROUNDWALL_LINE) && collideRoundWithRoundWall[pair<int, int>(e.t1, e.t2)] != e)
		) {
			continue;
		}

		eventsDone++;

		updateRoundObjectsForward(movingRoundObjects, e.time - knownTime);

		switch (e.type) {
			case ET_ROUND_ROUND:
				{
					collideRoundWithRound[pair<int, int>(e.t1, e.t2)] = collide_event(INFINITY, ET_NONE, e.t1, e.t2);

					MovingRoundObject& obj1 = *(movingRoundObjects[e.t1]);
					MovingRoundObject& obj2 = *(movingRoundObjects[e.t2]);

					Vector2D normal = obj2.center - obj1.center;

					collisionPoint = obj1.center+normal*(1/sqrt(normal*normal))*obj1.radius;
					pair<bool,bool> shouldDie = roundRoundCollisionCallback(
							ObjectPtr<MovingRoundObject>(&obj1),
							ObjectPtr<MovingRoundObject>(&obj2)
							);

					if (obj1.state == MOS_SHRINKING) {
						bounceMovingRoundAndShrinkingRound(obj1, obj2, shouldDie.second);
					}

					else if (obj2.state == MOS_SHRINKING) {
						bounceMovingRoundAndShrinkingRound(obj2, obj1, shouldDie.first);
					}

					else {
						if (shouldDie.first && shouldDie.second) {
							Vector2D vel = normal*(1/sqrt(normal*normal))*DEATH_RATE;
							obj1.startShrinking(NULL, vel);
							obj2.startShrinking(NULL, -vel);
						} else if (shouldDie.first) {
							obj2.velocity += obj1.velocity * (obj1.mass / obj2.mass);
							obj1.startShrinking(NULL, Vector2D(0.0f, 0.0f));
						} else if (shouldDie.second) {
							obj1.velocity += obj2.velocity * (obj2.mass / obj1.mass);
							obj2.startShrinking(NULL, Vector2D(0.0f, 0.0f));
						} else {

							float nv1 = obj1.velocity * normal;
							float nv2 = obj2.velocity * normal;
							obj1.velocity -=
								(nv1/(normal*normal))*normal -
								(((obj1.mass-obj2.mass)/(obj1.mass+obj2.mass)*nv1+2*obj2.mass/(obj1.mass+obj2.mass)*nv2)/(normal*normal))*normal;
							obj2.velocity -=
								(nv2/(normal*normal))*normal -
								(((obj2.mass-obj1.mass)/(obj2.mass+obj1.mass)*nv2+2*obj1.mass/(obj2.mass+obj1.mass)*nv1)/(normal*normal))*normal;

						}
					}

					for (auto i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
						if (i->first != obj1.getID() && i->first != obj2.getID()) {
							MovingRoundObject& obj = *(i->second);
							doObjectCollision(obj, obj1, collideRoundWithRound, collideEvents, e.time, dt);
							doObjectCollision(obj, obj2, collideRoundWithRound, collideEvents, e.time, dt);
						}
					}
					for (auto i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
						RectangularWall& wall = *(i->second);
						doRectangularWallCollision(
								obj1, wall, collideRoundWithWall, collideEvents, e.time, dt);
						doRectangularWallCollision(
								obj2, wall, collideRoundWithWall, collideEvents, e.time, dt);
					}
					for (auto i = roundWalls.begin(); i != roundWalls.end(); i++) {
						RoundWall& wall = *(i->second);
						doRoundWallCollision(
								obj1, wall, collideRoundWithRoundWall, collideEvents, e.time, dt);
						doRoundWallCollision(
								obj2, wall, collideRoundWithRoundWall, collideEvents, e.time, dt);
					}
					doRoundObjectDisappearing(obj1, collideDisappear, collideEvents, e.time, dt);
					doRoundObjectDisappearing(obj2, collideDisappear, collideEvents, e.time, dt);

					break;
				}

			case ET_ROUND_WALL_CORNER_1:
			case ET_ROUND_WALL_CORNER_2:
			case ET_ROUND_WALL_LINE:
				{
					collideRoundWithWall[pair<int, int>(e.t1, e.t2)] =
						collide_event(INFINITY, ET_NONE, e.t1, e.t2);
					MovingRoundObject& obj = *(movingRoundObjects[e.t1]);
					RectangularWall& wall = *(rectangularWalls[e.t2]);

					Vector2D normal;
					if (e.type == ET_ROUND_WALL_CORNER_1) {
						normal = obj.center - wall.p1;
					}
					else if (e.type == ET_ROUND_WALL_CORNER_2) {
						normal = obj.center - wall.p2;
					}
					else if (e.type == ET_ROUND_WALL_LINE) {
						normal = (wall.p2 - wall.p1).getNormalVector();
						if (obj.velocity * normal > 0) {
							normal = -normal;
						}
					} else {
						assert(false);
					}

					collisionPoint = obj.center+normal*(1/sqrt(normal*normal))*obj.radius;
					bool shouldDie = roundWallCollisionCallback(
							ObjectPtr<MovingRoundObject>(&obj),
							ObjectPtr<Wall>(&wall)
							);

					if (shouldDie) {
						obj.startShrinking(NULL, -normal*(1/sqrt(normal*normal))*DEATH_RATE);
					} else {
						bounceMovingRoundObjectByNormal(obj, normal, wall.bouncinessCoefficient);
					}

					for (auto i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
						if (i->first != e.t1) {
							doObjectCollision(obj, *(i->second), collideRoundWithRound, collideEvents, e.time, dt);
						}
					}
					for (auto i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
						if (i->first != e.t2) {
							doRectangularWallCollision(obj, *(i->second), collideRoundWithWall, collideEvents, e.time, dt);
						}
					}
					for (auto i = roundWalls.begin(); i != roundWalls.end(); i++) {
						doRoundWallCollision(obj, *(i->second), collideRoundWithRoundWall, collideEvents, e.time, dt);
					}
					doRoundObjectDisappearing(obj, collideDisappear, collideEvents, e.time, dt);

					break;
				}

			case ET_ROUND_ROUNDWALL_CORNER_1:
			case ET_ROUND_ROUNDWALL_CORNER_2:
			case ET_ROUND_ROUNDWALL_LINE:
				{
					collideRoundWithRoundWall[pair<int, int>(e.t1, e.t2)] = 
						collide_event(INFINITY, ET_NONE, e.t1, e.t2);
					MovingRoundObject& obj = *(movingRoundObjects[e.t1]);
					RoundWall& wall = *(roundWalls[e.t2]);

					Vector2D normal;
					if (e.type == ET_ROUND_ROUNDWALL_CORNER_1) {
						normal = obj.center - (wall.center +
								wall.radius * Vector2D(cosf(wall.theta1), sinf(wall.theta1)));

					}
					else if (e.type == ET_ROUND_ROUNDWALL_CORNER_2) {
						normal = obj.center - (wall.center +
								wall.radius * Vector2D(cosf(wall.theta2), sinf(wall.theta2)));
					}
					else if (e.type == ET_ROUND_ROUNDWALL_LINE) {
						normal = obj.center - wall.center;
						if (obj.velocity * normal > 0) {
							normal = -normal;
						}
					}

					collisionPoint = obj.center+normal*(1/sqrt(normal*normal))*obj.radius;
					bool shouldDie = roundWallCollisionCallback(
							ObjectPtr<MovingRoundObject>(&obj),
							ObjectPtr<Wall>(&wall)
							);

					if (shouldDie) {
						obj.startShrinking(NULL, -normal*(1/sqrt(normal*normal))*DEATH_RATE);
					} else {
						bounceMovingRoundObjectByNormal(obj, normal, wall.bouncinessCoefficient);
					}

					for (auto i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
						if (i->first != e.t1) {
							doObjectCollision(obj, *(i->second), collideRoundWithRound, collideEvents, e.time, dt);
						}
					}
					for (auto i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
						doRectangularWallCollision(obj, *(i->second), collideRoundWithWall, collideEvents, e.time, dt);
					}
					for (auto i = roundWalls.begin(); i != roundWalls.end(); i++) {
						doRoundWallCollision(obj, *(i->second), collideRoundWithRoundWall, collideEvents, e.time, dt);
					}
					doRoundObjectDisappearing(obj, collideDisappear, collideEvents, e.time, dt);

					break;
				}


			case ET_ROUND_DISAPPEAR:
				{
					collideDisappear[e.t1] = collide_event(INFINITY, ET_NONE, e.t1, -1);
					MovingRoundObject& obj = *(movingRoundObjects[e.t1]);

					MovingRoundObject *parent = obj.getShrinkingParent();
					obj.kill();

					for (auto i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
						if (i->first != obj.getID() && (parent == NULL || i->first != parent->getID())) {
							MovingRoundObject& obj = *(i->second);
							doObjectCollision(obj, obj, collideRoundWithRound, collideEvents, e.time, dt);
							if (parent != NULL) {
								doObjectCollision(obj, *parent, collideRoundWithRound, collideEvents, e.time, dt);
							}
						}
					}
					for (auto i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
						RectangularWall& wall = *(i->second);
						doRectangularWallCollision(obj, wall, collideRoundWithWall, collideEvents, e.time, dt);
						if (parent != NULL) {
							doRectangularWallCollision(*parent, wall, collideRoundWithWall, collideEvents, e.time, dt);
						}
					}
					for (auto i = roundWalls.begin(); i != roundWalls.end(); i++) {
						RoundWall& wall = *(i->second);
						doRoundWallCollision(obj, wall, collideRoundWithRoundWall, collideEvents, e.time, dt);
						if (parent != NULL) {
							doRoundWallCollision(*parent, wall, collideRoundWithRoundWall, collideEvents, e.time, dt);
						}
					}
					doRoundObjectDisappearing(obj, collideDisappear, collideEvents, e.time, dt);
					if (parent != NULL) {
						doRoundObjectDisappearing(*parent, collideDisappear, collideEvents, e.time, dt);
					}

					break;
				}

			case ET_NONE:
				{
					assert(false); // should never get here
				}
		}

		knownTime = e.time;
	}

	float elapsed = dt;
	if (!collideEvents.empty() && collideEvents.top().time < elapsed) {
		elapsed = knownTime;
	} else {
		updateRoundObjectsForward(movingRoundObjects, elapsed - knownTime);
	}

	// Spawning.
	// For simplicity, we do this on the boundary of a simulation interval.
	// It won't screw up physics if an object spawns slightly "late",
	// whereas things like collisions need to happen in order.
	for (auto& iter : movingRoundObjects) {
		MovingRoundObject& obj = *(iter.second);
		if (obj.state != MOS_SPAWNING) {
			continue;
		}

		obj.timeUntilSpawn -= elapsed;
		if (obj.timeUntilSpawn > 0) {
			continue;
		}

		if (rand() / float(RAND_MAX) >= SPAWN_PROB) {
			continue;
		}

		SpawnDescriptor const& spawnDesc =
			worldMap.getSpawnByNumber(obj.regionNumber);
		obj.center = spawnDesc.getRandomPoint();

		// check that we aren't intersecting any other objects if we spawn here
		bool okayToSpawn = true;
		for (auto& jter : movingRoundObjects) {
			MovingRoundObject& obj2 = *(jter.second);
			if (objectsIntersect(obj, obj2)) {
				okayToSpawn = false;
				break;
			}

		}
		if (!okayToSpawn) {
			continue;
		}
		for (auto& jter : rectangularWalls) {
			RectangularWall& wall = *(jter.second);
			if (objectsIntersect(obj, wall)) {
				okayToSpawn = false;
				break;
			}
		}
		if (!okayToSpawn) {
			continue;
		}

		obj.velocity = Vector2D(0.0f, 0.0f);
		obj.state = MOS_ALIVE;

		obj.onSpawnCallback();
	}
}
