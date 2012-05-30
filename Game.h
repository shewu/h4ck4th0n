#ifndef GAME_H
#define GAME_H

#include "socket.h"
#include "server.h"

#include "hack.h"
#include "World.h"

class Player {
    public:
        Client cl;
        int object_id;
        char key_pressed;
        float angle;
        double time_until_spawn;
        int team;
};

class Game {
    private:
    	HBMap hbmap;
        World world;

        std::map<int, Player> players[2];
        std::vector<std::pair<char, Vector2D> > sounds;

        SocketConnection *sc;

    public:
        Game();

        bool add_player(Client cl);
        void remove_player(int id);

        void send_world();
        void send_sounds_to(SocketConnection* c);
        void process_packet(int id, ReadPacket *rp);

        void update(float dt);
};

#endif
