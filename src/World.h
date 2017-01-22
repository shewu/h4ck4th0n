#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <vector>

#include "Object.h"
#include "HBMap.h"
#include "Light.h"

class World {
private:
    std::string mapName;

    std::vector<Light> lights;

protected:
    std::map<int, std::shared_ptr<MovingRoundObject> > movingRoundObjects;
    std::map<int, std::shared_ptr<RectangularWall> > rectangularWalls;
    std::map<int, std::shared_ptr<RoundWall> > roundWalls;
    HBMap worldMap;

public:
    /**
     * Constructs an empty world, should not be used.
     */
    World();

    /**
     * Default constructor for World given a map name from which to load
     * the world.
     *
     * @param mapName the map file name. Defaults to "custom.hbm".
     */
    World(HBMap const &);

    HBMap const &getMap() const { return worldMap; }

    /**
     * Returns the minimum x coordinate of the world.
     *
     * @return the minimum x coordinate.
     */
    float getMinX() const { return worldMap.getWidth() / -2.0f; }

    /**
     * Returns the maximum x coordinate of the world.
     *
     * @return the maximum x coordinate.
     */
    float getMaxX() const { return worldMap.getWidth() / 2.0f; }

    /**
     * Returns the minimum y coordinate of the world.
     *
     * @return the minimum y coordinate.
     */
    float getMinY() const { return worldMap.getHeight() / -2.0f; }

    /**
     * Returns the maximum y coordinate of the world.
     *
     * @return the maximum y coordinate.
     */
    float getMaxY() const { return worldMap.getHeight() / 2.0f; }

    /**
     * Returns the lights in the world.
     *
     * @return the lights.
     */
    std::vector<Light> const &getLights() const { return lights; }

    std::map<int, std::shared_ptr<MovingRoundObject> > const &getMovingRoundObjects() const {
        return movingRoundObjects;
    }

    std::map<int, std::shared_ptr<RectangularWall> > const &getRectangularWalls() const {
        return rectangularWalls;
    }

    std::map<int, std::shared_ptr<RoundWall> > const &getRoundWalls() const {
        return roundWalls;
    }
};

enum ObjectType { OT_FLAG_OBJECT, OT_PLAYER_OBJECT };

#endif
