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
#include "Hack.h"
#include "Logging.h"
#include "Packet.h"
#include "Socket.h"
#include "Sound.h"

using std::cin;
using std::cout;
using std::fstream;
using std::set;

#define MYPORT "55555"

#define TIMESTEP_MICROSECONDS 10000

struct Client {
	static int next_id;
	Client(SocketConnection* sc) {
		this->sc = sc;
		playerID = next_id++;
	}

	SocketConnection* sc;
	int playerID;

	bool operator<(Client const& cl) const {
		return playerID < cl.playerID;
	}
};
int Client::next_id = 0;

// A map from the ID numbers to the Client structs
set<Client> clients;

// Global variable keeping track of time
timeval tim;

// A Socket class which returns SocketConnection
// instances as clients connect
Socket *s;

void verify() {
    if(FRICTION < 0.0f) {
        cout << "Your friction is negative and will cause the game to fail\n";
		cout << "Continue anyway? (y/n) ";
		char c;
		cin >> c;
		if(!(c == 'y' || c == 'Y'))
			exit(-1);
    }
}

// Remove a client from the game
void remove_client(Client const& cl) {
    s->closeConnection(cl.sc);
    clients.erase(cl);
}

int main() {
    verify();

    fstream f;
    f.open("logs/log", fstream::out | fstream::binary);
    if (f.fail()) {
    	fprintf(stderr, "failed to open log, aborting\n");
    	return 0;
	}

	// since we log the random seed and use it for playback purposes, the
	// Server should not use randomness outside of Game
	unsigned int randomSeed = time(NULL);
	logRandomSeed(f, randomSeed);
	srand(randomSeed);

	// Initialize tim
    gettimeofday(&tim, NULL);

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

	printf("Server started, hit 'q' to quit.\n");

	WritePacket worldWritePacket(STC_WORLD_DATA);

	// TODO make it possible to set this at runtime
	Game* game = new CTFGame(HBMap("custom.hbm", "CTF"));
	game->init();

	// Main server loop
    while(true) {

		// Server should terminate if 'q' is passed to stdin.
		// Check for this
        pollfd fds;
        fds.fd = 0; //0 means stdin
        fds.events = POLLIN;
        if(poll(&fds, 1, 0)) { //do we have input from stdin
            char c = fgetc(stdin);
            if(c == 'q') {
                break;
            }
        }

		// Receive all incoming packets
		// Socket distributes these packets to
		// the appropriate SocketConnection instances
		s->recv_all();

		// Checking for new connections from clients
        SocketConnection *sc = s->receiveConnection();

        if(sc != NULL) {
            printf("Connection made\n");

            clients.insert(Client(sc));

            printf("Client added to clients map\n");
        }

		// Loop through all clients, and see if there are
		// any messages to be received.
        for(auto it = clients.begin(); it != clients.end(); ) {
            auto next_it = it;
            ++next_it;

            Client const& cl = *it;

			// First, we see when the last time we received
			// a packet from them is. If it was too long ago,
			// we disconnect. The contract between the client
			// and server requires that the client send a message
			// at least once every TIMEOUT seconds to demonstrate
			// that it is still present.
            if(cl.sc->lastTimeReceived < time(NULL) - TIMEOUT) {
				WritePacket wp(STC_DISCONNECT, 0);
				cl.sc->send_packet(wp);

				logRemovePlayer(f, cl.playerID);
                game->removePlayer(cl.playerID);
                remove_client(cl);
                printf("Client timed out, currently %zu clients\n",
                        clients.size());
            }

			// If the client is still here, then we read any packets
			// that we have from it. Most packet types* are dealt
			// with by game. The only packets we have to handle here
			// are the CTS_CONNECT and CTS_DISCONNECT messages.
			//
			// * the CTS_USER_STATE type
            else {
                ReadPacket* rp;
                while((rp = cl.sc->receive_packet()) != NULL) {
                    // Connect
                    if(rp->message_type == CTS_CONNECT) {
                        printf("Received connect message\n");

						WritePacket wp(STC_INIT_INFO, 4);
						logAddPlayer(f, cl.playerID);
                        if(game->addPlayer(cl.playerID, wp)) {
                        	cl.sc->send_packet(wp);
                            printf("Player added to game; currently %zu clients\n",
                                     clients.size());
						}
                    }

                    // Disconnect
                    else if(rp->message_type == CTS_DISCONNECT) {
                    	logRemovePlayer(f, cl.playerID);
                        game->removePlayer(cl.playerID);
                        remove_client(cl);
                        printf("Player disconnected, currently %zu clients\n",
											clients.size());
						break; // Don't keep polling a deleted SocketConnection
                    }

					// Give packet to game for processing
					// if the packet is of any other type.
                    else if (rp->message_type == CTS_USER_STATE &&
                             rp->packet_number >= cl.sc->largestPacketNum) {
                        logProcessPacket(f, cl.playerID, rp);
                        game->processPacket(cl.playerID, rp);
                    }

                    delete rp;
                }
            }

            it = next_it;
        }

		// Time stuff
        timeval tim2;
        gettimeofday(&tim2, NULL);
        float dt = (float)(tim2.tv_sec - tim.tv_sec) +
                (float)(tim2.tv_usec - tim.tv_usec) * 1.0e-6f;
        tim = tim2;

		// Update game state, e.g., do physics
		// computations
		logDoSimulation(f, dt);
        game->update(dt);

		// Send the world state to all clients
		worldWritePacket.reset();
		game->getWorld().writeToPacket(&worldWritePacket);
		for(auto iter = clients.begin();
		        iter != clients.end(); ++iter) {
		    Client const& cl = *iter;
		    int objectID = game->getObjectIDOf(cl.playerID);
		    if (objectID != Game::kNoPlayerExists) {
				worldWritePacket.write_int(objectID);
				cl.sc->send_packet(worldWritePacket);
				worldWritePacket.backup(sizeof(int));
			}
		}

		// Send the sounds to all clients
		for (Sound const& sound : game->getSounds()) {
			WritePacket wp(STC_SOUND, 9);
			sound.writeToPacket(&wp);
			for (Client const& cl : clients) {
				cl.sc->send_packet(wp);
			}
		}
		game->clearSounds();

		// Sleep a little while before the
		// next iteration.
        usleep(TIMESTEP_MICROSECONDS);
    }

	// kill server
	// should probably also deal with any clients
	close(sockfd);

	logDone(f);
	f.close();
}
