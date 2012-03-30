#include <fstream>
#include <map>
#include <string>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "world.h"

using namespace std;

World::World(std::string mapName) : wallColor(101, 67, 33), worldMap(mapName) {
	minX = -worldMap.getWidth()/2;
	maxX = worldMap.getWidth() + minX;
	minY = -worldMap.getHeight()/2;
	maxY = worldMap.getHeight() + minY;

    vector<Obstacle> walls = worldMap.getWalls();
    for (iterator<Obstacle> it = walls.begin(); it != walls.end(); ++it) {
        obstacles.push_back(*it);
    }

    vector<Team> teams = worldMap.getTeams();
    for (iterator<Team> team = teams.begin(); team != teams.end(); ++it) {
        ;
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
