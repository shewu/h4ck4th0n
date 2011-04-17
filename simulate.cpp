#include "hack.h"
#include <queue>
#include <cmath>
#include <stdio.h>

#define MAX_EVENTS 10

using namespace std;

float collideCircles(Vector2D diff, float r, Vector2D vel);

struct collide_event
{
	float time;
	int type;
	int t1, t2;
};

bool operator<(collide_event a, collide_event b) {
	return (a.time > b.time);
}

float collideCircles(Vector2D diff, float r, Vector2D vel, float rc)
{
	if (diff*vel+rc*sqrt(diff*diff) >= 0) return INFINITY;
	float a = vel*vel-rc*rc, b = 2*(diff*vel)+2*rc*r, c = diff*diff-r*r;
	float discrim = b*b-4*a*c;
	if (discrim <= 0) return INFINITY;
	float ans = (-b-sqrt(discrim))/(2*a);
	if (ans <= 0) return 0;
	return ans;
}

void doObjectCollision(int f, int s, Object fo, Object so, map<pair<int, int>, float>& collideTimes, priority_queue<collide_event>& collide_events, float cur, float dt) {
	if ((fo.dead && fo.stopped) || (so.dead && so.stopped) || (fo.dead && so.dead)) {
		collideTimes[pair<int, int>(f, s)] = INFINITY;
		return;
	}
	
	float rc = 0;
	Vector2D v(0, 0);
	if (fo.dead && fo.nattached == 0) {
		rc += DEATH_RATE;
		v -= fo.v;
	}
	else if (!fo.dead) v = -fo.v;
	if (so.dead && so.nattached == 0) {
		rc += DEATH_RATE;
		v += so.v;
	}
	else if (!so.dead) v += so.v;
	collideTimes[pair<int, int>(f, s)] = collideCircles(so.p-fo.p, fo.rad+so.rad, v, rc)+cur;
	if (collideTimes[pair<int, int>(f, s)] < dt) {
		collide_event e = { collideTimes[pair<int, int>(f, s)], 0, f, s };
		collide_events.push(e);
	}
}

void doObstacleCollision(int f, Object fo, vector<Obstacle>& obstacles, map<pair<int, int>, float>& collideTimesObs, map<pair<int, int>, float>& collideTypes, priority_queue<collide_event>& collide_events, float cur, float dt) {
	if (fo.dead) {
		if (!fo.stopped && fo.nattached == 0) {
			collideTypes[pair<int, int>(f, -1)] = -1;
			collideTimesObs[pair<int, int>(f, -1)] = fo.rad/DEATH_RATE;
			collide_event e = { collideTimesObs[pair<int, int>(f, -1)], -1, f, -1 };
			collide_events.push(e);
		}
		for (int j = 0; j < obstacles.size(); j++) collideTimesObs[pair<int, int>(f, j)] = INFINITY;
		return;
	}
	
	for (int j = 0; j < obstacles.size(); j++) {
		float time1 = collideCircles(fo.p-obstacles[j].p1, fo.rad, fo.v, 0), time2 = collideCircles(fo.p-obstacles[j].p2, fo.rad, fo.v, 0);
		float time3;
		Vector2D dir = obstacles[j].p2-obstacles[j].p1;
		Vector2D relpos = fo.p-obstacles[j].p1;
		Vector2D normal = dir.getNormalVector();
		float dist = relpos*normal;
		if (dist < 0) {
			normal = -normal;
			dist = -dist;
		}
		if (fo.v*normal >= 0) time3 = INFINITY;
		else {
			time3 = (fo.rad-dist)/(fo.v*normal);
			if (time3 < 0) time3 = 0;
			Vector2D newpos = relpos+time3*fo.v;
			if (newpos*dir < 0 || newpos*dir > dir*dir) time3 = INFINITY;
		}
		int type;
		if (time1 < time2 && time1 < time3) {
			collideTimesObs[pair<int, int>(f, j)] = time1+cur;
			type = 1;
		}
		else if (time2 < time3) {
			collideTimesObs[pair<int, int>(f, j)] = time2+cur;
			type = 2;
		}
		else {
			collideTimesObs[pair<int, int>(f, j)] = time3+cur;
			type = 3;
		}
		collideTypes[pair<int, int>(f, j)] = type;
		if (collideTimesObs[pair<int, int>(f, j)] < dt) {
			collide_event e = { collideTimesObs[pair<int, int>(f, j)], type, f, j };
			collide_events.push(e);
		}
	}
}

void World::doSimulation(float dt)
{
	map<pair<int, int>, float> collideTimes;
	map<pair<int, int>, float> collideTimesObs;
	map<pair<int, int>, float> collideTypes;
	priority_queue<collide_event> collide_events;
	for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) {
		for (map<int, Object>::iterator j = objects.begin(); j != objects.end(); j++) if (i->first < j->first) {
			doObjectCollision(i->first, j->first, i->second, j->second, collideTimes, collide_events, 0, dt);
		}
		
		doObstacleCollision(i->first, i->second, obstacles, collideTimesObs, collideTypes, collide_events, 0, dt);
	}
	
	int eventsDone = 0;
	float knownTime = 0;
	sounds.clear();
	while (!collide_events.empty() && collide_events.top().time < dt && eventsDone < MAX_EVENTS) {
		collide_event e = collide_events.top();
		collide_events.pop();
		if ((e.type == 0 && collideTimes[pair<int, int>(e.t1, e.t2)] != e.time) || (e.type != 0 && (collideTimesObs[pair<int, int>(e.t1, e.t2)] != e.time || collideTypes[pair<int, int>(e.t1, e.t2)] != e.type))) continue;
		eventsDone++;
		switch (e.type) {
			case 0: {
				collideTimes[pair<int, int>(e.t1, e.t2)] = INFINITY;
				if (!objects[e.t1].dead || objects[e.t1].nattached == 0) objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				if (!objects[e.t2].dead || objects[e.t2].nattached == 0) objects[e.t2].p += (e.time-knownTime)*objects[e.t2].v;
				Vector2D normal = objects[e.t2].p-objects[e.t1].p;
				float nv1 = objects[e.t1].v*normal, nv2 = objects[e.t2].v*normal;
				if (objects[e.t1].dead) {
					if (objects[e.t1].nattached == 0) objects[e.t1].rad -= DEATH_RATE*(e.time-knownTime);
					if ((objects[e.t1].flag == -1 && objects[e.t2].flag == -1) || (objects[e.t1].flag != -1 && objects[e.t2].flag != -1 && objects[e.t1].flag != objects[e.t2].flag)) {
						objects[e.t2].dead = true;
						objects[e.t2].stopped = false;
						objects[e.t2].spawny = 0;
						objects[e.t2].v = -normal*(1/sqrt(normal*normal))*DEATH_RATE;
						objects[e.t2].nattached = 0;
						objects[e.t2].attachedTo = e.t1;
						objects[e.t1].nattached++;
						if (sounds.size() < MAX_SOUNDS) if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(1+(objects[e.t1].flag != -1), objects[e.t1].p+normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
					}
					else {
						if (objects[e.t1].nattached != 0) objects[e.t2].v -= 2*(nv2/(normal*normal))*normal;
						else objects[e.t2].v -= 2*((nv2-nv1)/(normal*normal)+(1/sqrt(normal*normal))*DEATH_RATE)*normal;
						if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(0, objects[e.t1].p+normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
					}
				}
				else if (objects[e.t2].dead) {
					if (objects[e.t2].nattached == 0) objects[e.t2].rad -= DEATH_RATE*(e.time-knownTime);
					if ((objects[e.t1].flag == -1 && objects[e.t2].flag == -1) || (objects[e.t1].flag != -1 && objects[e.t2].flag != -1 && objects[e.t1].flag != objects[e.t2].flag)) {
						objects[e.t1].dead = true;
						objects[e.t1].stopped = false;
						objects[e.t1].spawny = 0;
						objects[e.t1].v = normal*(1/sqrt(normal*normal))*DEATH_RATE;
						objects[e.t1].nattached = 0;
						objects[e.t1].attachedTo = e.t2;
						objects[e.t2].nattached++;
						if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(1+(objects[e.t1].flag != -1), objects[e.t1].p+normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
					}
					else {
						if (objects[e.t2].nattached != 0) objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
						else objects[e.t1].v -= 2*((nv1-nv2)/(normal*normal)-(1/sqrt(normal*normal))*DEATH_RATE)*normal;
						if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(0, objects[e.t1].p+normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
					}
				}
				else {
					objects[e.t1].v -= (nv1/(normal*normal))*normal;
					objects[e.t2].v -= (nv2/(normal*normal))*normal;
					objects[e.t1].v += (((objects[e.t1].mass-objects[e.t2].mass)/(objects[e.t1].mass+objects[e.t2].mass)*nv1+2*objects[e.t2].mass/(objects[e.t1].mass+objects[e.t2].mass)*nv2)/(normal*normal))*normal;
					objects[e.t2].v += (((objects[e.t2].mass-objects[e.t1].mass)/(objects[e.t2].mass+objects[e.t1].mass)*nv2+2*objects[e.t1].mass/(objects[e.t2].mass+objects[e.t1].mass)*nv1)/(normal*normal))*normal;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(0, objects[e.t1].p+normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1 && i->first != e.t2) {
					if (!i->second.dead || (!i->second.stopped && i->second.nattached)) i->second.p += (e.time-knownTime)*i->second.v;
					if (i->second.dead && !i->second.stopped && i->second.nattached == 0) i->second.rad -= (e.time-knownTime)*DEATH_RATE;
					int f = e.t1, s = i->first;
					if (f > s) {
						int temp = f;
						f = s;
						s = temp;
					}
					doObjectCollision(f, s, objects[f], objects[s], collideTimes, collide_events, e.time, dt);
					f = e.t2, s = i->first;
					if (f > s) {
						int temp = f;
						f = s;
						s = temp;
					}
					doObjectCollision(f, s, objects[f], objects[s], collideTimes, collide_events, e.time, dt);
				}
				doObstacleCollision(e.t1, objects[e.t1], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
				doObstacleCollision(e.t2, objects[e.t2], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
				break;
			}
			case 1: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				Vector2D normal = objects[e.t1].p-obstacles[e.t2].p1;
				if (obstacles[e.t2].sticky && ((objects[e.t1].flag == -1 && obstacles[e.t2].flag == -1) || (objects[e.t1].flag != -1 && obstacles[e.t2].flag != -1 && objects[e.t1].flag != obstacles[e.t2].flag))) {
					objects[e.t1].dead = true;
					objects[e.t1].stopped = false;
					objects[e.t1].spawny = 0;
					objects[e.t1].v = -normal*(1/sqrt(normal*normal))*DEATH_RATE;
					objects[e.t1].nattached = 0;
					objects[e.t1].attachedTo = -1;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(1+(objects[e.t1].flag != -1), objects[e.t1].p-normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				else {
					float nv1 = objects[e.t1].v*normal;
					objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(0, objects[e.t1].p-normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					if (!i->second.dead || (!i->second.stopped && i->second.nattached == 0)) i->second.p += (e.time-knownTime)*i->second.v;
					if (i->second.dead && !i->second.stopped && i->second.nattached == 0) i->second.rad -= (e.time-knownTime)*DEATH_RATE;
					int f = e.t1, s = i->first;
					if (f > s) {
						int temp = f;
						f = s;
						s = temp;
					}
					doObjectCollision(f, s, objects[f], objects[s], collideTimes, collide_events, e.time, dt);
				}
				doObstacleCollision(e.t1, objects[e.t1], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
				break;
			}
			case 2: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				Vector2D normal = objects[e.t1].p-obstacles[e.t2].p2;
				if (obstacles[e.t2].sticky && ((objects[e.t1].flag == -1 && obstacles[e.t2].flag == -1) || (objects[e.t1].flag != -1 && obstacles[e.t2].flag != -1 && objects[e.t1].flag != obstacles[e.t2].flag))) {
					objects[e.t1].dead = true;
					objects[e.t1].stopped = false;
					objects[e.t1].spawny = 0;
					objects[e.t1].v = -normal*(1/sqrt(normal*normal))*DEATH_RATE;
					objects[e.t1].nattached = 0;
					objects[e.t1].attachedTo = -1;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(1+(objects[e.t1].flag != -1), objects[e.t1].p-normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				else {
					float nv1 = objects[e.t1].v*normal;
					objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(0, objects[e.t1].p-normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					if (!i->second.dead || (!i->second.stopped && i->second.nattached == 0)) i->second.p += (e.time-knownTime)*i->second.v;
					if (i->second.dead && !i->second.stopped && i->second.nattached == 0) i->second.rad -= (e.time-knownTime)*DEATH_RATE;
					int f = e.t1, s = i->first;
					if (f > s) {
						int temp = f;
						f = s;
						s = temp;
					}
					doObjectCollision(f, s, objects[f], objects[s], collideTimes, collide_events, e.time, dt);
				}
				doObstacleCollision(e.t1, objects[e.t1], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
				break;
			}
			case 3: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				Vector2D normal = (obstacles[e.t2].p2-obstacles[e.t2].p1).getNormalVector();
				if (obstacles[e.t2].sticky && ((objects[e.t1].flag == -1 && obstacles[e.t2].flag == -1) || (objects[e.t1].flag != -1 && obstacles[e.t2].flag != -1 && objects[e.t1].flag != obstacles[e.t2].flag))) {
					objects[e.t1].dead = true;
					objects[e.t1].stopped = false;
					objects[e.t1].spawny = 0;
					if (objects[e.t1].v*normal > 0) normal = -normal;
					objects[e.t1].v = -normal*(1/sqrt(normal*normal))*DEATH_RATE;
					objects[e.t1].nattached = 0;
					objects[e.t1].attachedTo = -1;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(1+(objects[e.t1].flag != -1), objects[e.t1].p-normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				else {
					float nv1 = objects[e.t1].v*normal;
					objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
					if (sounds.size() < MAX_SOUNDS) sounds.push_back(pair<int, Vector2D>(0, objects[e.t1].p-normal*(1/sqrt(normal*normal)*objects[e.t1].rad)));
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					if (!i->second.dead || (!i->second.stopped && i->second.nattached == 0)) i->second.p += (e.time-knownTime)*i->second.v;
					if (i->second.dead && !i->second.stopped && i->second.nattached == 0) i->second.rad -= (e.time-knownTime)*DEATH_RATE;
					int f = e.t1, s = i->first;
					if (f > s) {
						int temp = f;
						f = s;
						s = temp;
					}
					doObjectCollision(f, s, objects[f], objects[s], collideTimes, collide_events, e.time, dt);
				}
				doObstacleCollision(e.t1, objects[e.t1], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
				break;
			}
			case -1: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = -1;
				objects[e.t1].rad = 0;
				objects[e.t1].stopped = true;
				if (objects[e.t1].attachedTo != -1) objects[objects[e.t1].attachedTo].nattached--;
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1 && i->first != objects[e.t1].attachedTo) {
					if (!i->second.dead || (!i->second.stopped && i->second.nattached == 0)) i->second.p += (e.time-knownTime)*i->second.v;
					if (i->second.dead && !i->second.stopped && i->second.nattached == 0) i->second.rad -= (e.time-knownTime)*DEATH_RATE;
					int f = e.t1, s = i->first;
					if (f > s) {
						int temp = f;
						f = s;
						s = temp;
					}
					doObjectCollision(f, s, objects[f], objects[s], collideTimes, collide_events, e.time, dt);
					if (objects[e.t1].attachedTo != -1 && objects[objects[e.t1].attachedTo].nattached == 0) {
						int f2 = objects[e.t1].attachedTo, s2 = i->first;
						if (f2 > s2) {
							int temp = f2;
							f2 = s2;
							s2 = temp;
						}
						doObjectCollision(f2, s2, objects[f2], objects[s2], collideTimes, collide_events, e.time, dt);
					}
				}
				doObstacleCollision(e.t1, objects[e.t1], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
				if (objects[e.t1].attachedTo != -1 && objects[objects[e.t1].attachedTo].nattached == 0) doObstacleCollision(objects[e.t1].attachedTo, objects[objects[e.t1].attachedTo], obstacles, collideTimesObs, collideTypes, collide_events, e.time, dt);
			}

		}
		knownTime = e.time;
	}
	
	float elapsed = dt;
	if (!collide_events.empty() && collide_events.top().time < elapsed) elapsed = collide_events.top().time;
	for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) {
		if (!i->second.dead || (!i->second.stopped && i->second.nattached == 0)) i->second.p += (elapsed-knownTime)*i->second.v;
		if (i->second.dead && !i->second.stopped && i->second.nattached == 0) i->second.rad -= (elapsed-knownTime)*DEATH_RATE;
	}
}
