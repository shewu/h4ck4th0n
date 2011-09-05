#ifndef GAME_H
#define GAME_H

#include "socket.h"
#include "server.h"

enum {
    RED,
    BLU,
};

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
        World world;
        int team_count[2];

        std::map<int, Player> players;

        SocketConnection *sc;

    public:
        Game();

        bool add_player(Client cl);
        void remove_player(int id);

        void send_world();
        void process_packet(int id, char *buf, int length);

        void update(float dt);
};

#endif
