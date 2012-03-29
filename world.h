#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <vector>

#include "hack.h"
#include "HBMap.h"

class World
{
	private:
		std::string mapName;
        Color wallColor;
        HBMap worldMap;
	public:
		World(std::string mapName = "custom.hbm");
		int spawn(int spawnl, int player, int flag);
		
		float minX, maxX, minY, maxY;
		std::map<int, std::vector<Spawn> > spawns;
		std::map<int, Object> objects;
		std::vector<Light> lights;
		std::vector<Obstacle> obstacles;
		//std::vector<std::pair<char, Vector2D> > sounds;
		void doSimulation(float dt, std::vector<std::pair<char, Vector2D> >& sounds);
		
		void sendObjects(SocketConnection* sc, int obj);
		void receiveObjects(ReadPacket* rp, int& obj);
};

#endif

