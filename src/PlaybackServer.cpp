#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>

#include "CTFGame.h"
#include "Game.h"
#include "Logging.h"
#include "Packet.h"
#include "Socket.h"
#include "Sound.h"

using std::cin;
using std::cout;
using std::fstream;
using std::set;

#define MYPORT "55555"

// Global variable keeping track of time
timeval tim;

// A Socket class which returns SocketConnection
// instances as clients connect
Socket *s;

set<SocketConnection *> watchers;

int main() {
    fstream f;
    f.open("logs/log", fstream::in | fstream::binary);
    if (f.fail()) {
        fprintf(stderr, "failed to open log, aborting\n");
        return 0;
    }

    // Get a socket file descriptor, and make
    // a Socket instance
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, MYPORT, &hints, &res);

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    s = new Socket(sockfd);
    s->listen_for_client();

    // TODO make it possible to set this at runtime
    Game *game = new CTFGame(HBMap("custom.hbm", "CTF"));
    game->init();

    char buf[4];

    // Main server loop
    bool quit = false;
    while (!quit) {
        s->recv_all();
        SocketConnection *sc = s->receiveConnection();
        if (sc != NULL) {
            watchers.insert(sc);

            printf("watcher added\n");
        }

        for (SocketConnection *sc : watchers) {
            ReadPacket *rp;
            while ((rp = sc->receive_packet()) != NULL) {
                if (rp->message_type == CTS_CONNECT) {
                    WritePacket wp(STC_INIT_INFO, 0);
                    wp.write_float(0.0f);
                    game->getMap().writeToPacket(wp);
                    sc->send_packet(wp);
                }
                delete rp;
            }
        }

        char type = f.get();

        switch (type) {
            case kLoggingRemovePlayer: {
                int playerID;
                f.read(buf, 4);
                memcpy((void *)&playerID, buf, 4);

                game->removePlayer(playerID);
            }

            break;

            case kLoggingAddPlayer: {
                int playerID;
                f.read(buf, 4);
                memcpy((void *)&playerID, buf, 4);

                // not used, but need to pass as arg
                WritePacket wp(STC_INIT_INFO, 0);
                game->addPlayer(playerID, wp);
            }

            break;

            case kLoggingProcessPacket: {
                char message_type = f.get();

                int size;
                f.read(buf, 4);
                memcpy((void *)&size, buf, 4);

                int playerID;
                f.read(buf, 4);
                memcpy((void *)&playerID, buf, 4);

                int packet_number;
                f.read(buf, 4);
                memcpy((void *)&packet_number, buf, 4);

                ReadPacket rp(message_type, size, packet_number);
                f.read(rp.buf, size);

                game->processPacket(playerID, &rp);
            }

            break;

            case kLoggingDoSimulation: {
                float dt;
                f.read(buf, 4);
                memcpy((void *)&dt, buf, 4);

                game->update(dt);
                usleep((int)(dt * 1000000.0));

                // Send the world state to all watchers
                WritePacket worldWritePacket(STC_WORLD_DATA);
                game->getWorld().writeToPacket(&worldWritePacket);
                worldWritePacket.write_int(Game::kNoObjectExists);
                for (SocketConnection *sc : watchers) {
                    sc->send_packet(worldWritePacket);
                }

                // Send the sounds to all watchers
                for (Sound const &sound : game->getSounds()) {
                    WritePacket wp(STC_SOUND, 9);
                    sound.writeToPacket(&wp);
                    for (SocketConnection *sc : watchers) {
                        sc->send_packet(wp);
                    }
                }
                game->clearSounds();
            }

            break;

            case kLoggingDone: {
                quit = true;
            }

            break;

            case kLoggingRandomSeed: {
                unsigned int seed;
                f.read(buf, 4);
                memcpy((void *)&seed, buf, 4);

                srand(seed);
            }

            break;

            default: {
                fprintf(stderr, "error: unrecognized log code\n");
                quit = true;
            }

            break;
        }
    }

    // kill server
    // should probably also deal with any clients
    close(sockfd);

    f.close();
}
