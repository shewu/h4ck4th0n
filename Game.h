#ifndef GAME_H
#define GAME_H

#include "NetworkProtocol.h"
#include "Socket.h"
#include "World.h"
#include "PhysicsWorld.h"
#include "Hack.h"

class Game {
    private:
        PhysicsWorld world_;

		// Make Game non-copyable by making this private.
		Game(Game const&);
		Game& operator=(Game const&);

    public:
        Game(HBMap const& hbmap) : world_(hbmap) { }

        virtual ~Game() { }

		bool addPlayer(SocketConnection*);
		void removePlayer(SocketConnection*);
		int getObjectIDOf(SocketConnection*);

		PhysicsWorld const& getWorld() const {
			return world_;
		}

        void send_sounds_to(SocketConnection* c);
        void process_packet(SocketConnection* sc, ReadPacket* rp);

        void update(float dt);

        virtual void doGameLogic();
        virtual void sendGameInfoToClients();
};

#endif
