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
        std::map<SocketConnection*, GamePlayer*> players;

        void applyForcesFromInput(float dt);

		// Make Game non-copyable by making this private.
		Game(Game const&);
		Game& operator=(Game const&);

		// interface that needs to be implemented by inheriting Game class
        virtual bool roundWallCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<RectangularWall>) = 0;
        virtual std::pair<bool, bool> roundRoundCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<MovingRoundObject>) = 0;
        virtual void doGameLogic() = 0;
        virtual GamePlayer* onPlayerAdded() = 0;
        virtual void onPlayerRemoved(GamePlayer*) = 0;
        virtual void onInit() = 0;

    protected:
        PhysicsWorld world_;

        std::vector<Sound> sounds;

    public:
        Game(HBMap const& hbmap);

        virtual ~Game() { }

        void init();

		bool addPlayer(SocketConnection*);
		void removePlayer(SocketConnection*);

		void processPacket(SocketConnection* sc, ReadPacket* rp);
		void update(float dt);

		/**
		 * @return object id, or kNoObjectExists or kNoPlayerExists
		 */
		int getObjectIDOf(SocketConnection*);
        static const int kNoObjectExists = -1;
        static const int kNoPlayerExists = -2;

		PhysicsWorld const& getWorld() const {
			return world_;
		}

        void send_sounds_to(SocketConnection* c);

		std::vector<Sound> const& getSounds() const {
			return sounds;
		}

		void clearSounds() {
			sounds.clear();
		}
};

#endif
