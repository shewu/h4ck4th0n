#include "hack.h"
#include <fstream>
#include <string>
#include <map>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
using namespace std;

World::World() {
	std::ifstream mapf("map");
	mapf >> minX >> maxX >> minY >> maxY;
	
	Obstacle w1, w2, w3, w4;
	w1.p1 = Vector2D(minX, minY);
	w1.p2 = Vector2D(maxX, minY);
	w1.color = Color(101, 67, 33);
	w1.sticky = false;
	obstacles.push_back(w1);
	w2.p1 = Vector2D(minX, maxY);
	w2.p2 = Vector2D(maxX, maxY);
	w2.color = Color(101, 67, 33);
	w2.sticky = false;
	obstacles.push_back(w2);
	w3.p1 = Vector2D(minX, minY);
	w3.p2 = Vector2D(minX, maxY);
	w3.color = Color(101, 67, 33);
	w3.sticky = false;
	obstacles.push_back(w3);
	w4.p1 = Vector2D(maxX, minY);
	w4.p2 = Vector2D(maxX, maxY);
	w4.color = Color(101, 67, 33);
	w4.sticky = false;
	obstacles.push_back(w4);
	
	std::string cmd;
	while (mapf >> cmd) {
		if (cmd == "obs") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
			Obstacle obs;
			obs.p1 = Vector2D(x1, y1);
			obs.p2 = Vector2D(x2, y2);
			obs.color = Color(101, 67, 33);
			obs.sticky = false;
			obs.flag = -1;
			obstacles.push_back(obs);
		}
		else if (cmd == "obs0") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
			Obstacle obs;
			obs.p1 = Vector2D(x1, y1);
			obs.p2 = Vector2D(x2, y2);
			obs.color = Color(0, 0, 200);
			obs.sticky = true;
			obs.flag = 0;
			obstacles.push_back(obs);
		}
		else if (cmd == "obs1") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
			Obstacle obs;
			obs.p1 = Vector2D(x1, y1);
			obs.p2 = Vector2D(x2, y2);
			obs.color = Color(200, 0, 0);
			obs.sticky = true;
			obs.flag = 1;
			obstacles.push_back(obs);
		}
		else if (cmd == "sobs") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
			Obstacle obs;
			obs.p1 = Vector2D(x1, y1);
			obs.p2 = Vector2D(x2, y2);
			obs.color = Color(0, 255, 0);
			obs.sticky = true;
			obs.flag = -1;
			obstacles.push_back(obs);
		}
		else if (cmd == "spawn") {
			int spawn_id;
			float xmin, xmax, ymin, ymax, mass, rad, hrat;
			int r, g, b;
			mapf >> spawn_id >> xmin >> xmax >> ymin >> ymax >> mass >> rad >> hrat >> r >> g >> b;
			Spawn s;
			s.xmin = xmin;
			s.xmax = xmax;
			s.ymin = ymin;
			s.ymax = ymax;
			s.mass = mass;
			s.rad = rad;
			s.hrat = hrat;
			s.color = Color(r, g, b);
			spawns[spawn_id].push_back(s);
		}
		else if (cmd == "obj") {
			int spawnl;
			mapf >> spawnl;
			while (spawn(spawnl, -1, -1) == -1);
		}
	}
	
	Light l;
	l.position = Vector3D(0, 0, 10);
	l.color = Color(255, 255, 255);
	lights.push_back(l);
	
	Light l2;
	l2.position = Vector3D(0, 30, 20);
	l2.color = Color(255, 255, 255);
	lights.push_back(l2);
	
	while (spawn(1, -1, 0) == -1);
	while (spawn(3, -1, 1) == -1);
}

int World::spawn(int spawnl, int player, int flag) {
	if (rand()/float(RAND_MAX) >= SPAWN_PROB) return -1;

    //printf("passed randomness\n");
	
	int oid;
	do {
		oid = rand();
	} while (objects.count(oid));
	
	vector<Spawn>& sps = spawns[spawnl];
	int which = rand()%sps.size();
	
	Object o;
	o.v = Vector2D(0.0f, 0.0f);
	o.mass = sps[which].mass;
	o.rad = sps[which].rad;
	o.color = sps[which].color;
	o.hrat = sps[which].hrat;
	o.dead = false;
	o.id = oid;
	o.player = player;
	o.flag = flag;
	o.spawnl = spawnl;
	float min_x = sps[which].xmin + o.rad;
	float max_x = sps[which].xmax - o.rad;
	float min_y = sps[which].ymin + o.rad;
	float max_y = sps[which].ymax - o.rad;
	
	o.p.x = rand()/float(RAND_MAX)*(max_x - min_x) + min_x;
	o.p.y = rand()/float(RAND_MAX)*(max_y - min_y) + min_y;
	bool fail = false;
    //printf("about to enter loop\n");
	for (map<int, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		Vector2D d = it->second.p - o.p;
		if ((!it->second.dead || !it->second.stopped) && (it->second.rad + o.rad) * (it->second.rad + o.rad) > d*d) {
			fail = true;
			break;
		}
	}
    //printf("out of loop, fail = %c\n", fail ? 't' : 'f');
	if (fail)
		return -1;
	
	objects.insert(pair<int, Object>(o.id, o));
	return oid;
}

void World::sendObjects(SocketConnection* sc, int obj) {
	int o = htonl(obj);
	sc->add((char*)(&o), 4);
	
	int a = htonl(objects.size());
	sc->add((char*)(&a), 4);
	
	int s = objects.size();
	for(map<int, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		it->second.send(sc);
		s--;
	}
	
	int b = htonl(sounds.size());
	sc->add((char*)(&b), 4);
	for(vector<pair<char, Vector2D> >::iterator it = sounds.begin(); it != sounds.end(); it++) {
		char buf[9];
		buf[0] = it->first;
		*((int*)(buf+1)) = htonl(*reinterpret_cast<int*>(&it->second.x));
		*((int*)(buf+5)) = htonl(*reinterpret_cast<int*>(&it->second.y));
		sc->add(buf, 9);
	}
	sc->send();
}

int World::receiveObjects(SocketConnection* sc, int& obj) {
	char buffer[MAXPACKET];
	int len;
	if ((len = sc->receive(buffer, MAXPACKET)) < 12) return (len >= 0 ? 0 : -1);
	
	int numObjects = ntohl(*reinterpret_cast<int*>(buffer+4));
	if (numObjects < 0 || numObjects > 2*MAX_CLIENTS || len < 35*numObjects+12) return 0;
	
	int numSounds = ntohl(*reinterpret_cast<int*>(buffer+35*numObjects+8));
	if (numSounds < 0 || numSounds > MAX_EVENTS || len != 35*numObjects+9*numSounds+12) return 0;
	
	obj = ntohl(*reinterpret_cast<int*>(buffer));
	
	objects.clear();
	char* c = buffer+8;
	for (int i = 0; i < numObjects; i++) {
		Object o;
		c = o.get(c);
		objects[o.id] = o;
	}
	
	sounds.clear();
	sounds.reserve(numSounds);
	c += 4;
	for (int i = 0; i < numSounds; i++) {
		*((int*)(c+1)) = ntohl(*((int*)(c+1)));
		*((int*)(c+5)) = ntohl(*((int*)(c+5)));
		sounds.push_back(pair<char, Vector2D>(c[0], Vector2D(*reinterpret_cast<float*>(c+1), *reinterpret_cast<float*>(c+5))));
		c += 9;
	}
	return true;
}
