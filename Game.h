#ifndef GAME_H
#define GAME_H

#include "NetworkProtocol.h"
#include "Socket.h"
#include "World.h"
#include "PhysicsWorld.h"
#include "Hack.h"
#include "UserInput.h"
#include "Sound.h"

struct GamePlayer {
	UserInput input;
	ObjectPtr<MovingRoundObject> obj;
};

class Game {
    private:
    	// map from playerID to GamePlayer*
        std::map<int, GamePlayer*> players;

        void applyForcesFromInput(float dt);

		// Make Game non-copyable by making this private.
		Game(Game const&);
		Game& operator=(Game const&);

		// interface that needs to be implemented by inheriting Game class
		// to clarify, "roundWallCollision" means a collision between a
		// round object and a wall, not specifically a round wall
        virtual bool roundWallCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<Wall>) = 0;
        virtual std::pair<bool, bool> roundRoundCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<MovingRoundObject>) = 0;
        virtual void doGameLogic() = 0;
        virtual GamePlayer* onPlayerAdded() = 0;
        virtual void onPlayerRemoved(GamePlayer*) = 0;
        virtual void onInit() = 0;
        virtual std::string getScore(GamePlayer*) = 0;

    protected:
        PhysicsWorld world_;

        std::vector<Sound> sounds;

    public:
        Game(HBMap const& hbmap);

        virtual ~Game() { }

        void init();

		bool addPlayer(int playerID, WritePacket& initInfoWP);
		void removePlayer(int playerID);

		void processPacket(int playerID, ReadPacket* rp);
		void update(float dt);

        static const int kNoObjectExists = -1;
        static const int kNoPlayerExists = -2;
		/**
		 * @return object id, or kNoObjectExists or kNoPlayerExists
		 */
		int getObjectIDOf(int playerID);
		std::string getScoreByPlayerID(int playerID);

		PhysicsWorld const& getWorld() const {
			return world_;
		}

		std::vector<Sound> const& getSounds() const {
			return sounds;
		}

		void clearSounds() {
			sounds.clear();
		}

		HBMap const& getMap() {
			return world_.getMap();
		}
};

#endif
