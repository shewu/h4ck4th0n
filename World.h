#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <vector>

#include "Hack.h"
#include "Object.h"
#include "HBMap.h"
#include "Light.h"

class World
{
	private:
		std::string mapName;
        HBMap worldMap;

		std::vector<Light> lights;

	protected:
		std::map<int, MovingRoundObject*> movingRoundObjects;
		std::map<int, RectangularWall*> rectangularWalls;

	public:
        /**
         * Default constructor for World given a map name from which to load
         * the world.
         *
         * @param mapName the map file name. Defaults to "custom.hbm".
         */
		World(HBMap const& map);

		bool spawn(std::vector<SpawnDescriptor> const& possibleSpawns, MovingRoundObject& obj);

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
};

enum ObjectType {
	OT_FLAG_OBJECT,
	OT_PLAYER_OBJECT
};

#endif

