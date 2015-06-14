#include <fstream>
#include <map>
#include <string>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "World.h"

using namespace std;

World::World(HBMap const &hbmap) : worldMap(hbmap) {
    // rectangularWalls = worldMap.getRectangularWalls();

    Light l(Vector3D(0, 0, 10), Color(255, 255, 255));
    lights.push_back(l);

    Light l2(Vector3D(0, 30, 20), Color(255, 255, 255));
    lights.push_back(l2);
}
