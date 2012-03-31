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
        /**
         * Default constructor for World given a map name from which to load
         * the world.
         *
         * @param mapName the map file name. Defaults to "custom.hbm".
         */
		World(std::string mapName = "custom.hbm");
		int spawn(int spawnl, int player, int flag);

        /**
         * Returns the minimum x coordinate of the world.
         *
         * @return the minimum x coordinate.
         */
        const float getMinX() const {
            return worldMap.getWidth()/-2.0f;
        }

        /**
         * Returns the maximum x coordinate of the world.
         *
         * @return the maximum x coordinate.
         */
        const float getMaxX() const {
            return worldMap.getWidth()/2.0f;
        }

        /**
         * Returns the minimum y coordinate of the world.
         *
         * @return the minimum y coordinate.
         */
        const float getMinY() const {
            return worldMap.getHeight()/-2.0f;
        }

        /**
         * Returns the maximum y coordinate of the world.
         *
         * @return the maximum y coordinate.
         */
        const float getMaxY() const {
            return worldMap.getHeight()/2.0f;
        }
		
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

