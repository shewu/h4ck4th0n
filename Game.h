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
		int getObjectIDOf(SocketConnection*);
        void processPacket(SocketConnection* sc, ReadPacket* rp);

		PhysicsWorld const& getWorld() const {
			return world_;
		}

        void send_sounds_to(SocketConnection* c);

        void update(float dt);

        virtual void doGameLogic() = 0;
};

#endif
