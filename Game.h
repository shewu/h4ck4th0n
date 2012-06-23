#ifndef GAME_H
#define GAME_H

#include "Socket.h"
#include "Server.h"

#include "Hack.h"
#include "World.h"

class Game {
    private:
        PhysicsWorld world_;

        WritePacket* worldWritePacket_;

		// Make Game non-copyable by making this private.
		Game(Game const&);
		Game& operator=(Game const&);

    public:
        Game();
        ~Game();

		bool addPlayer();
		void removePlayer();

        void send_world();
        void send_sounds_to(SocketConnection* c);
        void process_packet(int id, ReadPacket *rp);

        void update(float dt);

        virtual void doGameLogic();
        virtual void sendGameInfoToClients();
};

#endif
