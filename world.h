#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <vector>

#include "hack.h"
#include "Object.h"
#include "HBMap.h"
#include "Light.h"

class World
{
	private:
		std::string mapName;
        Color wallColor;
        HBMap worldMap;
		std::vector<RectangularWall> rectWalls;
		std::vector<TeamDescriptor> teams;

		std::vector<Light> lights;

		std::map<int, MovingRoundObject> movingRoundObjects;
		std::set<RectangularWall> rectangularWalls;

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
        float getMinX() const {
            return worldMap.getWidth()/-2.0f;
        }

        /**
         * Returns the maximum x coordinate of the world.
         *
         * @return the maximum x coordinate.
         */
        float getMaxX() const {
            return worldMap.getWidth()/2.0f;
        }

        /**
         * Returns the minimum y coordinate of the world.
         *
         * @return the minimum y coordinate.
         */
        float getMinY() const {
            return worldMap.getHeight()/-2.0f;
        }

        /**
         * Returns the maximum y coordinate of the world.
         *
         * @return the maximum y coordinate.
         */
        float getMaxY() const {
            return worldMap.getHeight()/2.0f;
        }

		/**
		 * Returns the lights in the world.
		 *
		 * @return the lights.
		 */
        std::vector<Light> const& getLights() const {
        	return lights;
		}
		
		void doSimulation(float dt, std::vector<std::pair<char, Vector2D> >& sounds);
		
		void sendObjects(SocketConnection* sc, int obj);
		void receiveObjects(ReadPacket* rp, int& obj);
};

#endif

