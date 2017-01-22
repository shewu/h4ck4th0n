#ifndef GAME_H
#define GAME_H

#include "NetworkProtocol.h"
#include "Socket.h"
#include "World.h"
#include "PhysicsWorld.h"
#include "UserInput.h"
#include "Sound.h"

struct GamePlayer {
    UserInput input;
    std::shared_ptr<MovingRoundObject> obj;
};

class Game {
private:
    // map from playerID to GamePlayer*
    std::map<int, GamePlayer*> players;

    // helper method used by Game
    void applyForcesFromInput(float dt);

    // Make Game non-copyable by making this private.
    Game(Game const&);
    Game& operator=(Game const&);

    // interface that needs to be implemented by inheriting Game class
    // to clarify, "roundWallCollision" means a collision between a
    // round object and a wall, not specifically a round wall
    virtual RoundCollisionResult roundWallCollision(std::shared_ptr<MovingRoundObject>, std::shared_ptr<Wall>) = 0;
    virtual std::pair<RoundCollisionResult, RoundCollisionResult>roundRoundCollision(std::shared_ptr<MovingRoundObject>,
                        std::shared_ptr<MovingRoundObject>) = 0;
    virtual void doGameLogic() = 0;
    virtual GamePlayer* onPlayerAdded() = 0;
    virtual void onPlayerRemoved(GamePlayer*) = 0;
    virtual void onInit() = 0;
    virtual std::string getScore(GamePlayer*) = 0;

protected:
    // The PhysicsWorld
    PhysicsWorld world_;

    // Vector of sounds, the derived class can add to this with push_back
    std::vector<Sound> sounds;

public:
    Game(HBMap const& hbmap);
    virtual ~Game() {}

    // should be called before anything else
    void init();

    // can be called at any time by Game owner
    bool addPlayer(int playerID, WritePacket& initInfoWP);
    void removePlayer(int playerID);

    void processPacket(int playerID, ReadPacket* rp);
    void update(float dt);

    // some constants for the following getter methods
    static const int kNoObjectExists = -1;
    static const int kNoPlayerExists = -2;

    /**
     * @return object id, or kNoObjectExists or kNoPlayerExists
     */
    int getObjectIDOf(int playerID);
    std::string getScoreByPlayerID(int playerID);

    PhysicsWorld const& getWorld() const { return world_; }

    /**
     * @return the sounds in the sound buffer
     */
    std::vector<Sound> const& getSounds() const { return sounds; }

    /**
     * clears the sound buffer
     */
    void clearSounds() { sounds.clear(); }

    /**
     * @return the map
     */
    HBMap const& getMap() { return world_.getMap(); }
};

#endif
