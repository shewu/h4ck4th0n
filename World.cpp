#include <fstream>
#include <map>
#include <string>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "World.h"

using namespace std;

World::World(HBMap const& map) : worldMap(map) {
    //rectangularWalls = worldMap.getRectangularWalls();

	Light l(Vector3D(0,0,10), Color(255,255,255));
	lights.push_back(l);
	
	Light l2(Vector3D(0,30,20), Color(255,255,255));
	lights.push_back(l2);
}

/**
 * Attempts to spawn the given object at the given location.
 * @param possibleSpawns Possible places to spawn at.
 * @param obj The object to spawn. Should be in the MOS_SPAWNING state.
 * @return True if the spawn is successful.
 */
bool World::spawn(vector<SpawnDescriptor> const& possibleSpawns, MovingRoundObject& obj) {
	if (rand()/float(RAND_MAX) >= SPAWN_PROB) {
		return false;
	}

	SpawnDescriptor const& spawn = possibleSpawns[rand() % possibleSpawns.size()];

	Vector2D pos(random_uniform_float(spawn.getMinX(), spawn.getMaxX()),
	             random_uniform_float(spawn.getMinY(), spawn.getMaxY()));

	bool fail = false;
	for (map<int, MovingRoundObject *>::iterator it = movingRoundObjects.begin(); it != movingRoundObjects.end(); it++) {
		MovingRoundObject const& otherObj = *(it->second);
		Vector2D difference = obj.getCenter() - otherObj.getCenter();
		float minDistance = obj.getRadius() + otherObj.getRadius();
		if ((otherObj.getState() == MOS_ALIVE || otherObj.getState() == MOS_SHRINKING) &&
			             minDistance * minDistance > difference * difference) {
			fail = true;
			break;
		}
	}

	if(fail)
		return false;
}

// TODO networking
/*
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
*/
