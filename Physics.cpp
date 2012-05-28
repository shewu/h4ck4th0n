
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
 * @param collideTimes A map to add the time to with key (fo.getID(), so.getID())
 * @param collide_events A queue of events to add an event to if there is one.
 * @param cur The current time.
 * @param dt The amount of time to look into the future. Collision is not added to the queue
 *           if it occurs after dt time has passed.
 */
void doObjectCollision(MovingRoundObject const& fo, MovingRoundObject const& so,
                       map<pair<int,int>,float>& collideTimes,
                       priority_queue<collide_event>& collide_events,
                       float cur, float dt) {
	if (fo.getState() == MOS_DEAD || so.getState() == MOS_DEAD ||
		fo.getState() == MOS_SPAWNING || so.getState() == MOS_SPAWNING || 
		(fo.getState() == MOS_SHRINKING && so.getState() == MOS_SHRINKING)) {
		collideTimes[pair<int, int>(fo.getID(), so.getID())] = INFINITY;
		return;
	}

	float rc = 0.0;
	Vector2D v(0, 0);
	if (fo.getState() == MOS_ALIVE) {
		v -= fo.getVelocity();
	} else if (fo.getState() == MOS_SHRINKING && fo.getNumShrinkingChildren() == 0) {
		rc += DEATH_RATE;
	}
	if (so.getState() == MOS_ALIVE) {
		v += so.getVelocity();
	} else if (so.getState() == MOS_SHRINKING && so.getNumShrinkingChildren() == 0) {
		rc += DEATH_RATE;
	}
	
	float t = collideCircles(so.getCenter() - fo.getCenter(),
	                         so.getRadius() + fo.getRadius(), v, rc) + cur;
	collideTimes[pair<int, int>(fo.getID(), so.getID())] = t;
	if (t < dt) {
		collide_event e = { t, 0, fo.getID(), so.getID() };
		collide_events.push(e);
	}
}

void doRectangularWallCollision(MovingRoundObject const& obj,
                                vector<RectangularWall> const& walls,
                                map<pair<int, int>, float>& collideTimesWalls,
                                map<pair<int, int> float>& collideTypes,
                                priority_queue<collide_event>& collide_events,
                                float cur, float dt) {
    if (fo.getState() != MOS_ALIVE) {
		for (int j = 0; j < (int)
	}
}

void PhysicsWorld::doSimulation() {
	map<pair<int, int>, float> collideTimes;
	map<pair<int, int>, float> collideTimesObs;
	map<pair<int, int>, float> collideTypes;
	priority_queue<collide_event> collide_events;

	for (map<int, MovingRoundObject>::iterator i = movingRoundObjects.begin(); i != movingRoundObjects.end(); i++) {
		map<int, MovingRoundObject>::iterator j = i;
		for(j++; j != movingRoundObjects.end(); j++) {
			doObjectCollision(i->second, j->second, collideTimes, collide_events, 0, dt);
		}
	}
}
