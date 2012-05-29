enum EventType
{
	ET_NONE,
	ET_ROUND_ROUND,
	ET_ROUND_DISAPPEAR,
	ET_ROUND_WALL_CORNER_1,
	ET_ROUND_WALL_CORNER_2,
	ET_ROUND_WALL_LINE
};

struct collide_event
{
	float time;
	enum EventType type;

	/*
	 * Depends on type.
	 * ET_ROUND_ROUND: IDs of round objects
	 * ET_ROUND_DISAPPEAR: t1 is the ID of the object, t2 is unused.
	 * ET_ROUND_WALL_*: t1 is the ID of the round object, t2 ID of the wall.
	 */
	int t1, t2;
	bool operator<(collide_event const& a) const {
		return (a.time > b.time);
	}
	collide_event(float time, EventType type, int t1, int t2) :
	              time(time), type(type), t1(t1), t2(t2) { }
	bool operator==(collide_event const &a) const {
		return type == a.type && t1 == a.t1 && t2 == a.t2 && time == a.time;
	}
};

/**
 * Computes the time when two circle objects will intersect.
 * @param diff Center of B relative to A.
 * @param vel Velocity of B relative to A.
 * @param r Sum of radii of A and B.
 * @param rc Total rate at which r is decreasing.
 * @return The time at which the two objects will intersect if uninterrupted.
 */
float collideCircles(Vector2D diff, Vector2D vel, float r, float rc) {
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
void doObjectCollision(MovingRoundObject const& fo, MovingRoundObject const& so,
                       map<pair<int,int>, collide_event>& collideRoundWithRound,
                       priority_queue<collide_event>& collideEvents,
                       float cur, float dt) {
	if (fo.getState() == MOS_DEAD || so.getState() == MOS_DEAD ||
		fo.getState() == MOS_SPAWNING || so.getState() == MOS_SPAWNING || 
		(fo.getState() == MOS_SHRINKING && so.getState() == MOS_SHRINKING)) {
		if(fo.getID() < so.getID()) {
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
	if (fo.getState() == MOS_ALIVE || fo.getState() == MOS_SHRINKING) {
		v -= fo.getVelocity();
	} else if (fo.getState() == MOS_SHRINKING && fo.getNumShrinkingChildren() == 0) {
		rc += DEATH_RATE;
	}
	if (so.getState() == MOS_ALIVE || fo.getState() == MOS_SHRINKING) {
		v += so.getVelocity();
	} else if (so.getState() == MOS_SHRINKING && so.getNumShrinkingChildren() == 0) {
		rc += DEATH_RATE;
	}
	
	float t = collideCircles(so.getCenter() - fo.getCenter(), v,
	                         so.getRadius() + fo.getRadius(), rc) + cur;
	if(fo.getID() < so.getID()) {
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

void doRectangularWallCollision(MovingRoundObject const& obj,
                                RectangularWall const& wall,
                                map<pair<int, int>, collide_event>& collideRoundWithWall,
                                priority_queue<collide_event>& collideEvents,
                                float cur, float dt) {
    if (fo.getState() != MOS_ALIVE) {
		collideRoundWithWall[pair<int, int>(obj.getID(), wall.getID())] =
		        collide_event(INFINITY, ET_NONE, fo.getID(), so.getID());
		return;
	}

	float time1 = collideCircles(obj.getCenter() - wall.getP1(), obj.getVelocity(),
	                             obj.getRadius(), 0);
	float time2 = collideCircles(obj.getCenter() - wall.getP2(), obj.getVelocity(),
	                             obj.getRadius(), 0);

	float time3;
	Vector2D dir = wall.getP2() - wall.getP1();
	Vector2D relpos = obj.getCenter() - wall.getP1();
	Vector2D normal = dir.getNormalVector();
	float dist = relpos * normal;
	if (dist < 0) {
		normal = -normal;
		dist = -dist;
	}
	if (obj.getVelocity() * normal >= 0) {
		time3 = INFINITY;
	} else {
		time3 = (obj.getRadius() - dist) / (obj.getVelocity() * normal);
		if (time3 < 0) {
			time3 = 0;
		}
		Vector2D newpos = relpos + time3 * obj.getVelocity();
		if (newpos * dir < 0 || newpos * dir > dir*dir) {
			time3 = INFINITY;
		}
	}

	EventType type;
	float mintime;
	if (time1 < time2 && time1 < time3) {
		mintime = time1;
		type = ET_ROUND_WALL_CORNER_1;
	} else if(time2 < time3) {
		mintime = time2;
		type = ET_ROUND_WALL_CORNER_2;
	} else {
		mintime = time3;
		type = ET_ROUND_WALL_CORNER_LINE;
	}
	collision_event e(cur + mintime, type, obj.getID(), wall.getID());
	collideRoundWithWall[pair<int, int>(obj.getID(), wall.getID())] = e;
	if(mintime < dt) {
		collideEvents.push(e);
	}
}

void doRoundObjectDisappearing(MovingRoundObject const& obj,
                               map<int, collide_event>& collideDisappear,
                               priority_queue<collide_event>& collideEvents,
                               float cur, float dt) {
	if(obj.getState() == MOS_SHRINKING && obj.getNumShrinkingChildren() == 0) {
		float time = obj.getRadius() / DEATH_RATE;
		collision_event e(cur + t, ET_ROUND_DISAPPEAR, obj.getID(), -1);
		collideDisappear[obj.getID()] = e;
		if(t < dt) {
			collideEvents.push(e);
		}
	} else {
		collideDisappear[obj.getID()] = collision_event(INFINITY, ET_NONE, obj.getID(), -1);
	}
}

/**
 * What should happen if a obj2 is moving and hits obj1. Starts obj2 shrinking if necessary;
 * otherwise, updates its velocity.
 * @param obj1 A shrinking round object.
 * @param obj2 A moving object which hits obj1.
 */
void bounceMovingRoundAndShrinkingRound(MovingRoundObject& obj1, MovingRoundObject& obj2) {
	if (obj2.shouldDieFromShrinkingObject(obj1)) {
		obj2.startShrinking();
	} else {
		if (obj1.isCurrentlyShrinking()) {
			obj2.setVelocity(obj2.getVelocity() - 2.0 * ((nv2-nv1)/(normal*normal)+(1/sqrt(normal*normal))*DEATH_RATE)*normal);
		} else {
			obj2.setVelocity(obj2.getVelocity() - 2.0*(nv2 / (normal*normal)) * normal);
		}
	}
}

void bounceMovingRoundFromPoint(MovingRoundObject& obj, Vector2D const& p) {
	Vector2D normal = obj.getVelocity() - p;
	float nv1 = obj.getVelocity() * normal;
	obj.setVelocity(obj.getVelocity() - 2.0 * (nv1/(normal*normal)) * normal);
}

void bounceMovingRoundFromWall(MovingRoundObject& obj, Vector2D const& p1, Vector2D const& p2) {
	Vector2D normal = (p2 - p1).getNormalVector();
	float nv1 = obj.getVelocity() * normal;
	obj.setVelocity(obj.getVelocity() - 2.0 * (nv1/(normal*normal)) * normal);
}

void updateRoundObjectsForward(map<int, MovingRoundObject>& objects, float dt) {
	for(map<int, MovingRoundObject*>::iterator i = objects.begin(); i != objects.end(); i++) {
		MovingRoundObject& obj = *(i->second);
		if(obj.getState() == MOS_ALIVE) {
			obj.setCenter(obj.getCenter() + obj.getVelocity() * dt);
		}
		else if(obj.isCurrentlyShrinking()) {
			obj.setCenter(obj.getCenter() + obj.getVelocity() * dt);
			obj.setRadius(obj.getRadius() - DEATH_RATE * dt);
		}
	}
}

void PhysicsWorld::doSimulation() {
	map<pair<int, int>, collide_event> collideRoundWithRound;
	map<pair<int, int>, collide_event> collideRoundWithWall;
	map<int, collide_event> collideDisappear;
	priority_queue<collide_event> collideEvents;

	for (map<int, MovingRoundObject*>::iterator i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
		map<int, MovingRoundObject*>::iterator j = i;
		for (j++; j != movingRoundObjects.end(); j++) {
			doObjectCollision(*(i->second), *(j->second), collideRoundWithRound, collideEvents, 0, dt);
		}
		for (map<int, RectangularWall*>::iterator k = rectangularWalls.begin(); k != rectangularWalls.end(); k++) {
			doRectangularWallCollision(*(i->second), *(k->second), collideRoundWithWall, collideEvents, 0, dt);
		}
		doRoundObjectDisappearing(*(i->second), collideDisappear, collideEvents, 0, dt);
	}

	int eventsDone = 0;
	float knownTime = 0.0;
	sounds.clear();
	while (!collideEvents.empty() && collideEvents.top().time < dt && eventsDone < MAX_EVENTS) {
		collide_event e = collideEvents.top();
		collideEvents.pop();

		if ((e.type == ET_ROUND_ROUND     && collideRoundWithRound[pair<int, int>(e.t1, e.t2)] != e) ||
			(e.type == ET_ROUND_DISAPPEAR && collideRoundDisappear[pair<int, int>(e.t1, e.t2)] != e) ||
			((e.type == ET_ROUND_WALL_CORNER_1 || e.type == ET_ROUND_WALL_CORNER_2 || e.type == ET_ROUND_WALL_LINE) && collideRoundWithWall[pair<int, int>(e.t1, e.t2)] != e)) {
			continue;
		}

		eventsDone++;

		updateRoundObjectsForward(movingRoundObjects, e.time - knownTime);

		switch (e.type) {
			case ET_ROUND_ROUND:
				collideRoundWithRound[pair<int, int>(e.t1, e.t2)] = collide_event(INFINITY, ET_NONE, e.t1, e.t2);

				MovingRoundObject& obj1 = *(movingRoundObjects[e.t1]);
				MovingRoundObject& obj2 = *(movingRoundObjects[e.t2]);

				Vector2D normal = obj2.getCenter() - obj1.getCenter();
				float nv1 = obj1.getVelocity() * normal;
				float nv2 = obj2.getVelocity() * normal;

				if (obj1.getState() == MOS_SHRINKING) {
					bounceMovingRoundAndShrinkingRound(obj1, obj2);
				}

				else if(obj2.getState() == MOS_SHRINKING) {
					bounceMovingRoundAndShrinkingRound(obj2, obj1);
				}

				else {
					obj1.setVelocity(obj1.getVelocity() -
					                 (nv1/(normal*normal))*normal +
					                 (((obj1.mass-obj2.mass)/(obj1.mass+obj2.mass)*nv1+2*obj2.mass/(obj1.mass+obj2.mass)*nv2)/(normal*normal))*normal
					                );
					obj2.setVelocity(obj2.getVelocity() -
					                 (nv2/(normal*normal))*normal +
					                 (((obj2.mass-obj1.mass)/(obj2.mass+obj1.mass)*nv2+2*obj1.mass/(obj2.mass+obj1.mass)*nv1)/(normal*normal))*normal
					                );
				}

				for (map<int, MovingRoundObject*>::iterator i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
					if (i->first != obj1.getID() && i->first != obj2.getID()) {
						MovingRoundObject& obj = *(i->second);
						doObjectCollision(obj, obj1, collideRoundWithRound, collideEvents, e.time, dt);
						doObjectCollision(obj, obj2, collideRoundWithRound, collideEvents, e.time, dt);
					}
				}
				for (map<int, RectangularWall*>::iterator i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
					RectangularWall& wall = *(i->second);
					doRectangularWallCollision(obj1, wall, collideRoundWithWall, collideEvents, e.time, dt);
					doRectangularWallCollision(obj2, wall, collideRoundWithWall, collideEvents, e.time, dt);
				}
				doRoundObjectDisappearing(obj1, collideDisappear, collideEvents, e.time, dt);
				doRoundObjectDisappearing(obj2, collideDisappear, collideEvents, e.time, dt);

				break;

			case ET_ROUND_WALL_CORNER_1:
			case ET_ROUND_WALL_CORNER_2:
			case ET_ROUND_WALL_LINE:
				collideRoundWithWall[pair<int, int>(e.t1, e.t2)] = collide_event(INFINITY, ET_NONE, e.t1, e.t2);
				MovingRoundObject& obj = *(movingRoundObjects[e.t1]);
				RectangularWall& wall = *(rectangularWalls[e.t2]);

				if (obj.shouldDieFromWall(wall)) {
					obj.startShrinking();
				} else {
					if(e.type == ET_ROUND_WALL_CORNER_1) {
						bounceMovingRoundFromPoint(obj, wall.getP1());
					}
					else if(e.type == ET_ROUND_WALL_CORNER_2) {
						bounceMovingRoundFromPoint(obj, wall.getP2());
					}
					else if(e.type == ET_ROUND_WALL_LINE) {
						bounceMovingRoundFromWall(obj, wall.getP1(), wall.getP2());
					}
				}

				for (map<int, MovingRoundObject*>::iterator i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
					if(i->first != e.t1) {
						doObjectCollision(obj, i->second, collideRoundWithRound, collideEvents, e.time, dt);
					}
				}
				for (map<int, RectangularWall*>::iterator i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
					if(i->first != e.t2) {
						doRectangularWallCollision(obj, i->second, collideRoundWithWall, collideEvents, e.time, dt);
					}
				}
				doRoundObjectDisappearing(obj, collideDisappear, collideEvents, e.time, dt);

			case ET_ROUND_DISAPPEAR:
				collideDisappear[e.t1] = collide_event(INFINITY, ET_NONE, e.t1, -1);
				MovingRoundObject& obj = *(movingRoundObjects[e.t1]);

				MovingRoundObject *parent = obj.getShrinkingParent();
				obj.kill();

				for (map<int, MovingRoundObject*>::iterator i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
					if (i->first != obj.getID() && (parent == NULL || i->first != parent->getID())) {
						MovingRoundObject& obj = *(i->second);
						doObjectCollision(obj, obj, collideRoundWithRound, collideEvents, e.time, dt);
						if(parent != NULL) {
							doObjectCollision(obj, *parent, collideRoundWithRound, collideEvents, e.time, dt);
						}
					}
				}
				for (map<int, RectangularWall*>::iterator i = rectangularWalls.begin(); i != rectangularWalls.end(); i++) {
					RectangularWall& wall = *(i->second);
					doRectangularWallCollision(obj, wall, collideRoundWithWall, collideEvents, e.time, dt);
					if(parent != NULL) {
						doRectangularWallCollision(*parent, wall, collideRoundWithWall, collideEvents, e.time, dt);
					}
				}
				doRoundObjectDisappearing(obj, collideDisappear, collideEvents, e.time, dt);
				if(parent != NULL) {
					doRoundObjectDisappearing(*parent, collideDisappear, collideEvents, e.time, dt);
				}
		}

		knownTime = e.time;
	}

	float elapsed = dt;
	if (!collideEvents.empty() && collideEvents.top().time < elapsed) {
		elapsed = knownTime;
	}
	updateRoundObjectsForward(movingRoundObjects, elapsed);
}
