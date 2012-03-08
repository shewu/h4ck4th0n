#include "hack.h"
#include <fstream>
#include <string>
#include <map>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

World::parseMap(String filename) {
	const Color wallColor(101, 67, 33);
	std::ifstream mapf(filename);
	String str;

	// get the map name.
	// name "Vlad's map"
	getline(mapf, str);
	if (strcmp(str.substring(0, 4), "name") == 0 && strcmp(str.substring(str.length()-1, str.length()), "\"") == 0) {
		size_t pos = str.find('"');
		if (pos == str.length()-1) {
			throw new ParseException();
		}
		mapName = str.substring(pos, str.length()-1);
	} else {
		throw new ParseException();
	}

	// mode TAG CTF
	getline(mapf, str);
	str = str.substring(4, str.length());
	// dimensions # #
	getline(mapf, str);
	str = str.substring(10, str.length());
	// team # #-#
	getline(mapf, str);
	while (strcmp(str.substring(0, 4), "team") == 0) {
		str = str.substring(5, str.length());
		int a = 0, b = 0, c = 0, i = 0;
		while ('0' > str[i] || str[i] > '9') {
			++i;
		}
		// parse a
		while ('0' <= str[i] && str[i] <= '9') {
			a = (a * 10 + (str[i] - '0'));
			++i;
		}
		while ('0' > str[i] || str[i] > '9') {
			++i;
		}
		// parse b
		while ('0' <= str[i] && str[i] <= '9') {
			b = (b * 10 + (str[i] - '0'));
			++i;
		}
		++i; // advance over '-'
		// parse c
		while ('0' <= str[i] && str[i] <= '9') {
			c = (c * 10 + (str[i] - '0'));
			++i;
		}
		getline(mapf, str);
	}
	// spawn # # #
	while (strcmp(str.substring(0, 5), "spawn") == 0) {
		str = str.substring(5, str.length());
		int a = 0, b = 0, c = 0, i = 0;
		while ('0' > str[i] || str[i] > '9') {
			++i;
		}
		// parse a
		while ('0' <= str[i] && str[i] <= '9') {
			a = (a * 10 + (str[i] - '0'));
			++i;
		}
		while ('0' > str[i] || str[i] > '9') {
			++i;
		}
		// parse b
		while ('0' <= str[i] && str[i] <= '9') {
			b = (b * 10 + (str[i] - '0'));
			++i;
		}
		++i; // advance over '-'
		// parse c
		while ('0' <= str[i] && str[i] <= '9') {
			c = (c * 10 + (str[i] - '0'));
			++i;
		}
		getline(mapf, str);
	}
	// flag # # #
	while (strcmp(str.substring(0, 4), "flag") == 0) {
		str = str.substring(4, str.length());
		int a = 0, b = 0, c = 0, i = 0;
		while ('0' > str[i] || str[i] > '9') {
			++i;
		}
		// parse a
		while ('0' <= str[i] && str[i] <= '9') {
			a = (a * 10 + (str[i] - '0'));
			++i;
		}
		while ('0' > str[i] || str[i] > '9') {
			++i;
		}
		// parse b
		while ('0' <= str[i] && str[i] <= '9') {
			b = (b * 10 + (str[i] - '0'));
			++i;
		}
		++i; // advance over '-'
		// parse c
		while ('0' <= str[i] && str[i] <= '9') {
			c = (c * 10 + (str[i] - '0'));
			++i;
		}
		getline(mapf, str);
	}
	// wall str # # # #
	while (strcmp(str.substring(0, 4), "wall") == 0) {
		getline(mapf, str);
	}

	mapf.close();
}

World::World() {
	std::ifstream mapf("map.hbm");
	mapf >> minX >> maxX >> minY >> maxY;
	
	obstacles.push_back(Obstacle(Vector2D(minX, minY), Vector2D(maxX, minY), wall_color));
    obstacles.push_back(Obstacle(Vector2D(minX, maxY), Vector2D(maxX, maxY), wall_color));
    obstacles.push_back(Obstacle(Vector2D(minX, minY), Vector2D(minX, maxY), wall_color));
    obstacles.push_back(Obstacle(Vector2D(maxX, minY), Vector2D(maxX, maxY), wall_color));
	
    std::string cmd;

	while (mapf >> cmd) {
		if (cmd == "obs") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
            obstacles.push_back(Obstacle(Vector2D(x1, y1), Vector2D(x2, y2), wall_color));
		}
		else if (cmd == "obs0") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
            obstacles.push_back(Obstacle(Vector2D(x1, y1), Vector2D(x2, y2),
                        Color(0, 0, 200), true, BLU));
		}
		else if (cmd == "obs1") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
            obstacles.push_back(Obstacle(Vector2D(x1, y1), Vector2D(x2, y2),
                        Color(200, 0, 0), true, RED));
		}
		else if (cmd == "sobs") {
			float x1, y1, x2, y2;
			mapf >> x1 >> y1 >> x2 >> y2;
            obstacles.push_back(Obstacle(Vector2D(x1, y1), Vector2D(x2, y2),
                        Color(0, 255, 0), true));
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
			while (spawn(spawnl, -1, NO_TEAM) == -1);
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
	
	while (spawn(1, -1, BLU) == -1);
	while (spawn(3, -1, RED) == -1);
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
    WritePacket wp(STC_WORLD_DATA, 12 + 35 * objects.size());

    wp.write_int(obj);

    wp.write_int(objects.size());
    for(map<int, Object>::iterator it = objects.begin(); it != objects.end(); ++it) {
        it->second.write_data(&wp);
    }

    sc->send_packet(wp);
}

void World::receiveObjects(ReadPacket* rp, int& obj) {
    obj = rp->read_int();

    int numObjects = rp->read_int();
    objects.clear();
    for(int i = 0; i < numObjects; i++) {
        Object o;
        o.read_data(rp);
        objects[o.id] = o;
    }
}
