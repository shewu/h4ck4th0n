#include "hack.h"
#include <fstream>
#include <string>
#include <map>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
using namespace std;

World::World() {
	Obstacle w1, w2, w3, w4;
	w1.p1 = Vector2D(MIN_X, MIN_Y);
	w1.p2 = Vector2D(MAX_X, MIN_Y);
	w1.color = Color(255, 0, 0);
	w1.sticky = false;
	obstacles.push_back(w1);
	w2.p1 = Vector2D(MIN_X, MAX_Y);
	w2.p2 = Vector2D(MAX_X, MAX_Y);
	w2.color = Color(255, 0, 0);
	w2.sticky = false;
	obstacles.push_back(w2);
	w3.p1 = Vector2D(MIN_X, MIN_Y);
	w3.p2 = Vector2D(MIN_X, MAX_Y);
	w3.color = Color(255, 0, 0);
	w3.sticky = false;
	obstacles.push_back(w3);
	w4.p1 = Vector2D(MAX_X, MIN_Y);
	w4.p2 = Vector2D(MAX_X, MAX_Y);
	w4.color = Color(255, 0, 0);
	w4.sticky = false;
	obstacles.push_back(w4);
	
	std::ifstream mapf("map");
	int nobs = 0;
	std::string cmd;
	while (mapf >> cmd) {
		if (cmd == "obs") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
			Obstacle obs;
			obs.p1 = Vector2D(x1, y1);
			obs.p2 = Vector2D(x2, y2);
			obs.color = Color(255, 0, 0);
			obs.sticky = false;
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
			while (spawn(spawnl, -1) == -1);
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
}

int World::spawn(int spawnl, int player) {
	if (rand()/float(RAND_MAX) >= SPAWN_PROB) return -1;
	
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
	o.spawnl = spawnl;
	float min_x = sps[which].xmin + o.rad;
	float max_x = sps[which].xmax - o.rad;
	float min_y = sps[which].ymin + o.rad;
	float max_y = sps[which].ymax - o.rad;
	
	o.p.x = rand()/float(RAND_MAX)*(max_x - min_x) + min_x;
	o.p.y = rand()/float(RAND_MAX)*(max_y - min_y) + min_y;
	bool fail = false;
	for (map<int, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		Vector2D d = it->second.p - o.p;
		if ((!it->second.dead || !it->second.stopped) && (it->second.rad + o.rad) * (it->second.rad + o.rad) > d*d) {
			fail = true;
			break;
		}
	}
	if (fail)
		return -1;
	
	objects.insert(pair<int, Object>(o.id, o));
	return oid;
}

bool World::sendObjects(Socket socket, int obj) {
	int o = htonl(obj);
	if (!socket.send((char*)(&o), 4))
		return false;
	
	int a = htonl(objects.size());
	if (!socket.send((char*)(&a), 4))
		return false;
	
	int s = objects.size();
	for(map<int, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		if (!it->second.send(socket))
			return false;
		s--;
	}
	
	int b = htonl(sounds.size());
	if (!socket.send((char*)(&b), 4))
		return false;
	for(vector<pair<char, Vector2D> >::iterator it = sounds.begin(); it != sounds.end(); it++) {
		char buf[9];
		buf[0] = it->first;
		*((int*)(buf+1)) = htonl(*reinterpret_cast<int*>(&it->second.x));
		*((int*)(buf+5)) = htonl(*reinterpret_cast<int*>(&it->second.y));
		if (!socket.send(buf, 9))
			return false;
	}
	return true;
}

bool World::receiveObjects(Socket socket, int& obj) {
	if (!socket.receive((char*)(&obj), 4)) return false;
	obj = ntohl(obj);
	
	int a;
	if (!socket.receive((char*)(&a), 4)) return false;
	int numObjects = ntohl(a);
	objects.clear();
	for (int i = 0; i < numObjects; i++) {
		Object o;
		if(!o.receive(socket))
			return false;
		objects[o.id] = o;
	}
	
	int b;
	socket.receive((char*)(&b), 4);
	int numSounds = ntohl(b);
	sounds.clear();
	sounds.reserve(numSounds);
	for (int i = 0; i < numSounds; i++) {
		char buf[9];
		if (!socket.receive(buf, 9))
			return false;
		*((int*)(buf+1)) = ntohl(*((int*)(buf+1)));
		*((int*)(buf+5)) = ntohl(*((int*)(buf+5)));
		sounds.push_back(pair<char, Vector2D>(buf[0], Vector2D(*reinterpret_cast<float*>(buf+1), *reinterpret_cast<float*>(buf+5))));
	}
	return true;
}
