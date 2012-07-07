#ifndef GAME_H
#define GAME_H

#include "NetworkProtocol.h"
#include "Socket.h"
#include "World.h"
#include "PhysicsWorld.h"
#include "Hack.h"
#include "UserInput.h"

struct GamePlayer {
	UserInput input;
	ObjectPtr<PlayerObject> obj;
};

class Game {
    private:
        PhysicsWorld world_;

        std::map<SocketConnection*, GamePlayer> players;

        void applyForcesFromInput(float dt);

		// Make Game non-copyable by making this private.
		Game(Game const&);
		Game& operator=(Game const&);

    public:
        Game(HBMap const& hbmap) : world_(hbmap) { }

        virtual ~Game() { }

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

        virtual void doGameLogic() = 0;
        virtual void handleCollision(ObjectPtr<Object>, ObjectPtr<Object>) = 0;
};

#endif
