#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "World.h"

#include <functional>
#include <queue>
#include <utility>
#include <vector>

enum class RoundCollisionResult { NOTHING, DEATH };

class PhysicsWorld : public World {
public:
    typedef std::function<RoundCollisionResult(ObjectPtr<MovingRoundObject>,
                                               ObjectPtr<Wall>)>
        RoundWallCollisionCallback;
    typedef std::function<std::pair<RoundCollisionResult, RoundCollisionResult>(
        ObjectPtr<MovingRoundObject>, ObjectPtr<MovingRoundObject>)>
        RoundRoundCollisionCallback;

    PhysicsWorld(HBMap const& hbmap) : World(hbmap) {
        roundWallCollisionCallback =
            [](ObjectPtr<MovingRoundObject> a, ObjectPtr<Wall> b) {
            return RoundCollisionResult::NOTHING;
        };
        roundRoundCollisionCallback =
            [](ObjectPtr<MovingRoundObject> a, ObjectPtr<MovingRoundObject> b) {
            return std::make_pair(RoundCollisionResult::NOTHING,
                                  RoundCollisionResult::NOTHING);
        };
        for (auto const& wallDesc : hbmap.getRectangularWalls()) {
            RectangularWall* wall = new RectangularWall(
                wallDesc.getMaterial(), wallDesc.getWallType(),
                wallDesc.getBouncinessCoefficient(), wallDesc.getPos1(),
                wallDesc.getPos2());
            rectangularWalls.insert(std::make_pair(wall->getID(), wall));
        }
        for (auto const& wallDesc : hbmap.getRoundWalls()) {
            RoundWall* wall =
                new RoundWall(wallDesc.getMaterial(), wallDesc.getWallType(),
                              wallDesc.getBouncinessCoefficient(),
                              wallDesc.getCenter(), wallDesc.getRadius(),
                              wallDesc.getTheta1(), wallDesc.getTheta2());
            roundWalls.insert(std::make_pair(wall->getID(), wall));
        }
    }

    void applyForces(float dt);
    void doSimulation(float dt);

    /**
     * These must be safe to call from a callback during a simulation
     */
    ObjectPtr<MovingRoundObject> addFlagObject(
        int regionNumber, std::function<void()> const& onSpawnCallback,
        std::function<void()> const& onDeathCallback);
    ObjectPtr<MovingRoundObject> addPlayerObject(
        int regionNumber, std::function<void()> const& onSpawnCallback,
        std::function<void()> const& onDeathCallback);
    /**
     * When called from a collision callback, returns the point of collision.
     */
    Vector2D const& getCollisionPoint() const { return collisionPoint; }

    void removeDeadObjects();

    void writeToPacket(WritePacket* wp) const;

    void setRoundWallCollisionCallback(
        RoundWallCollisionCallback const& callback) {
        roundWallCollisionCallback = callback;
    }

    void setRoundRoundCollisionCallback(
        RoundRoundCollisionCallback const& callback) {
        roundRoundCollisionCallback = callback;
    }

private:
    Vector2D collisionPoint;

    RoundWallCollisionCallback roundWallCollisionCallback;
    RoundRoundCollisionCallback roundRoundCollisionCallback;

    enum EventType {
        ET_NONE,
        ET_ROUND_ROUND,
        ET_ROUND_DISAPPEAR,
        ET_ROUND_WALL_CORNER_1,
        ET_ROUND_WALL_CORNER_2,
        ET_ROUND_WALL_LINE,
        ET_ROUND_ROUNDWALL_CORNER_1,
        ET_ROUND_ROUNDWALL_CORNER_2,
        ET_ROUND_ROUNDWALL_LINE
    };

    struct collide_event {
        float time;
        enum EventType type;

        /*
         * Depends on type.
         * ET_ROUND_ROUND: IDs of round objects
         * ET_ROUND_DISAPPEAR: t1 is the ID of the object, t2 is unused.
         * ET_ROUND_WALL_*: t1 is the ID of the round object, t2 ID of the wall.
         */
        int t1, t2;

        collide_event() {}
        collide_event(float time, EventType type, int t1, int t2)
            : time(time), type(type), t1(t1), t2(t2) {}

        bool operator==(collide_event const& a) const {
            return type == a.type && t1 == a.t1 && t2 == a.t2 && time == a.time;
        }
        bool operator!=(collide_event const& a) const {
            return !(operator==(a));
        }
        bool operator<(collide_event const& a) const { return (time > a.time); }
    };

    // Helper methods for physics (see Physics.cpp)
    static bool objectsIntersect(MovingRoundObject const& obj1,
                                 MovingRoundObject const& obj2);
    static bool objectsIntersect(MovingRoundObject const& obj,
                                 RectangularWall const& wall);
    static float collideCircles(Vector2D diff, Vector2D vel, float r, float rc);
    static void doObjectCollision(
        MovingRoundObject const& fo, MovingRoundObject const& so,
        std::map<std::pair<int, int>, collide_event>& collideRoundWithRound,
        std::priority_queue<collide_event>& collideEvents, float cur, float dt);
    static void doRectangularWallCollision(
        MovingRoundObject const& obj, RectangularWall const& wall,
        std::map<std::pair<int, int>, collide_event>& collideRoundWithWall,
        std::priority_queue<collide_event>& collideEvents, float cur, float dt);
    static void doRoundWallCollision(
        MovingRoundObject const& obj, RoundWall const& wall,
        std::map<std::pair<int, int>, collide_event>& collideRoundWithRoundWall,
        std::priority_queue<collide_event>& collideEvents, float cur, float dt);
    static void doRoundObjectDisappearing(
        MovingRoundObject const& obj,
        std::map<int, collide_event>& collideDisappear,
        std::priority_queue<collide_event>& collideEvents, float cur, float dt);
    static void bounceMovingRoundAndShrinkingRound(MovingRoundObject& obj1,
                                                   MovingRoundObject& obj2,
                                                   bool shouldDie);
    static void bounceMovingRoundObjectByNormal(MovingRoundObject& obj,
                                                Vector2D const& normal,
                                                float bouncinessCoefficient);
    static void updateRoundObjectsForward(
        std::map<int, MovingRoundObject*>& objects, float dt);
};

#endif
