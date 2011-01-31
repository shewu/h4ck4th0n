#include "hack.h"
#include <queue>
#include <math.h>
#include <stdio.h>

#define MAX_EVENTS 1 /* Only 1 event can be handled per frame and still slightly broken */

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
	if (b*b-4*a*c <= 0) return INFINITY;
	float ans = (-b-sqrt(discrim))/(2*a);
	if (ans <= 0) return 0;
	return ans;
}

void World::doSimulation(float dt)
{
	map<pair<int, int>, float> collideTimes;
	map<pair<int, int>, float> collideTimesObs;
	priority_queue<collide_event> collide_events;
	for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) {
		for (map<int, Object>::iterator j = objects.begin(); j != objects.end(); j++) if (i->first < j->first) {
			collideTimes[pair<int, int>(i->first, j->first)] = collideCircles(j->second.p-i->second.p, i->second.rad+j->second.rad, j->second.v-i->second.v);
			if (collideTimes[pair<int, int>(i->first, j->first)] < dt) {
				collide_event e = { collideTimes[pair<int, int>(i->first, j->first)], 0, i->first, j->first };
				collide_events.push(e);
			}
		}
		
		for (int j = 0; j < obstacles.size(); j++) {
			float time1 = collideCircles(i->second.p-obstacles[j].p1, i->second.rad, i->second.v), time2 = collideCircles(i->second.p-obstacles[j].p2, i->second.rad, i->second.v);
			float time3;
			Vector2D dir = obstacles[j].p2-obstacles[j].p1;
			Vector2D relpos = i->second.p-obstacles[j].p1;
			Vector2D normal = dir.getNormalVector();
			float dist = relpos*normal;
			if (dist < 0) {
				normal = -normal;
				dist = -dist;
			}
			if (i->second.v*normal >= 0) time3 = INFINITY;
			else {
				time3 = (i->second.rad-dist)/(i->second.v*normal);
				if (time3 < 0) time3 = 0;
				Vector2D newpos = relpos+time3*i->second.v;
				if (newpos*dir < 0 || newpos*dir > dir*dir) time3 = INFINITY;
			}
			int type;
			if (time1 < time2 && time1 < time3) {
				collideTimesObs[pair<int, int>(i->first, j)] = time1;
				type = 1;
			}
			else if (time2 < time3) {
				collideTimesObs[pair<int, int>(i->first, j)] = time2;
				type = 2;
			}
			else {
				collideTimesObs[pair<int, int>(i->first, j)] = time3;
				type = 3;
			}
			if (collideTimes[pair<int, int>(i->first, j)] < dt) {
				collide_event e = { collideTimes[pair<int, int>(i->first, j)], type, i->first, j };
				collide_events.push(e);
			}
		}
	}
	
	int eventsDone = 0;
	float knownTime = 0;
	while (!collide_events.empty() && collide_events.top().time < dt && eventsDone < MAX_EVENTS) {
		collide_event e = collide_events.top();
		collide_events.pop();
		if ((e.type == 0 && collideTimes[pair<int, int>(e.t1, e.t2)] != e.time) || (e.type != 0 && collideTimesObs[pair<int, int>(e.t1, e.t2)] != e.time)) continue;
		eventsDone++;
		switch (e.type) {
			case 0: {
				collideTimes[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				objects[e.t2].p += (e.time-knownTime)*objects[e.t2].v;
				Vector2D normal = objects[e.t2].p-objects[e.t1].p;
				float nv1 = objects[e.t1].v*normal, nv2 = objects[e.t2].v*normal;
				objects[e.t1].v -= (nv1/(normal*normal))*normal;
				objects[e.t2].v -= (nv2/(normal*normal))*normal;
				objects[e.t1].v += (((objects[e.t1].mass-objects[e.t2].mass)/(objects[e.t1].mass+objects[e.t2].mass)*nv1+2*objects[e.t2].mass/(objects[e.t1].mass+objects[e.t2].mass)*nv2)/(normal*normal))*normal;
				objects[e.t2].v += (((objects[e.t2].mass-objects[e.t1].mass)/(objects[e.t2].mass+objects[e.t1].mass)*nv2+2*objects[e.t1].mass/(objects[e.t2].mass+objects[e.t1].mass)*nv1)/(normal*normal))*normal;
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1 && i->first != e.t2) {
					i->second.p += (e.time-knownTime)*i->second.v;
				}
				break;
			}
			case 1: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				Vector2D normal = objects[e.t1].p-obstacles[e.t2].p1;
				float nv1 = objects[e.t1].v*normal;
				objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					i->second.p += (e.time-knownTime)*i->second.v;
				}
				break;
			}
			case 2: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				Vector2D normal = objects[e.t1].p-obstacles[e.t2].p2;
				float nv1 = objects[e.t1].v*normal;
				objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					i->second.p += (e.time-knownTime)*i->second.v;
				}
				break;
			}
			case 3: {
				collideTimesObs[pair<int, int>(e.t1, e.t2)] = INFINITY;
				objects[e.t1].p += (e.time-knownTime)*objects[e.t1].v;
				Vector2D normal = (obstacles[e.t2].p2-obstacles[e.t2].p1).getNormalVector();
				float nv1 = objects[e.t1].v*normal;
				objects[e.t1].v -= 2*(nv1/(normal*normal))*normal;
				
				for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++) if (i->first != e.t1) {
					i->second.p += (e.time-knownTime)*i->second.v;
				}
				break;
			}

		}
		knownTime = e.time;
	}
	
	for (map<int, Object>::iterator i = objects.begin(); i != objects.end(); i++)
		i->second.p += (dt-knownTime)*i->second.v;
}
