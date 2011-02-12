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

float collideCircles(Vector2D diff, float r, Vector2D vel)
{
	if (diff*vel >= 0) return INFINITY;
	float a = vel*vel, b = 2*(diff*vel), c = diff*diff-r*r;
	float discrim = b*b-4*a*c;
	if (discrim <= 0) return INFINITY;
	float ans = (-b-sqrt(discrim))/(2*a);
	if (ans <= 0) return 0;
	return ans;
}

void doObjectCollision(int f, int s, Object fo, Object so, map<pair<int, int>, float>& collideTimes, priority_queue<collide_event>& collide_events, float cur, float dt) {
	if (fo.rad == 0 || so.rad == 0) return;
	collideTimes[pair<int, int>(f, s)] = collideCircles(so.p-fo.p, fo.rad+so.rad, so.v-fo.v)+cur;
	if (collideTimes[pair<int, int>(f, s)] < dt) {
		collide_event e = { collideTimes[pair<int, int>(s, f)], 0, s, f };
		collide_events.push(e);
	}
}

void doObstacleCollision(int f, Object fo, vector<Obstacle>& obstacles, map<pair<int, int>, float>& collideTimesObs, map<pair<int, int>, float>& collideTypes, priority_queue<collide_event>& collide_events, float cur, float dt) {
	for (int j = 0; j < obstacles.size(); j++) {
		float time1 = collideCircles(fo.p-obstacles[j].p1, fo.rad, fo.v), time2 = collideCircles(fo.p-obstacles[j].p2, fo.rad, fo.v);
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
	while (!collide_events.empty() && collide_events.top().time < dt && eventsDone < MAX_EVENTS) {
		collide_event e = collide_events.top();
		collide_events.pop();
		if ((e.type == 0 && collideTimes[pair<int, int>(e.t1, e.t2)] != e.time) || (e.type != 0 && (collideTimesObs[pair<int, int>(e.t1, e.t2)] != e.time || collideTypes[pair<int, int>(e.t1, e.t2)] != e.type))) continue;
		eventsDone++;
		switch (e.type) {
			case 0: {
				collideTimes[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				objects[e.t2].p += (e.time-knownTime)*objects[e.t2].v;
				Vector2D normal = objects[e.t2].p-objects[e.t1].p;
				float nv1 = objects[e.t1].v*normal, nv2 = objects[e.t2].v*normal;
				if (objects[e.t1].dead) objects[e.t2].v -= 2*(nv2/(normal*normal))*normal;
				else if (objects[e.t2].dead) objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				else {
					objects[e.t1].v -= (nv1/(normal*normal))*normal;
					objects[e.t2].v -= (nv2/(normal*normal))*normal;
					objects[e.t1].v += (((objects[e.t1].mass-objects[e.t2].mass)/(objects[e.t1].mass+objects[e.t2].mass)*nv1+2*objects[e.t2].mass/(objects[e.t1].mass+objects[e.t2].mass)*nv2)/(normal*normal))*normal;
					objects[e.t2].v += (((objects[e.t2].mass-objects[e.t1].mass)/(objects[e.t2].mass+objects[e.t1].mass)*nv2+2*objects[e.t1].mass/(objects[e.t2].mass+objects[e.t1].mass)*nv1)/(normal*normal))*normal;
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1 && i->first != e.t2) {
					i->second.p += (e.time-knownTime)*i->second.v;
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
				if (obstacles[e.t2].deadly) {
					objects[e.t1].dead = true;
					objects[e.t1].ddir = -normal*(1/sqrt(normal*normal));
					objects[e.t1].v = 0;
				}
				else {
					float nv1 = objects[e.t1].v*normal;
					objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					i->second.p += (e.time-knownTime)*i->second.v;
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
				if (obstacles[e.t2].deadly) {
					objects[e.t1].dead = true;
					objects[e.t1].ddir = -normal*(1/sqrt(normal*normal));
					objects[e.t1].v = 0;
				}
				else {
					float nv1 = objects[e.t1].v*normal;
					objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					i->second.p += (e.time-knownTime)*i->second.v;
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
				if (obstacles[e.t2].deadly) {
					objects[e.t1].dead = true;
					objects[e.t1].ddir = -normal*(1/sqrt(normal*normal));
					objects[e.t1].v = 0;
				}
				else {
					float nv1 = objects[e.t1].v*normal;
					objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				}
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					i->second.p += (e.time-knownTime)*i->second.v;
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

		}
		knownTime = e.time;
	}
	
	float elapsed = dt;
	if (!collide_events.empty() && collide_events.top().time < elapsed) elapsed = collide_events.top().time;
	for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++)
		i->second.p += (elapsed-knownTime)*i->second.v;
}
