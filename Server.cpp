
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
#include <iostream>
#include <set>

#include "CTFGame.h"
#include "Game.h"
#include "Hack.h"
#include "Packet.h"
#include "Socket.h"

using std::cin;
using std::cout;
using std::set;

#define MYPORT "55555"

#define TIMESTEP_MICROSECONDS 10000

/* Every connecting client is randomly assigned
   a unique client ID number. The SocketConnection
   corresponding to that client, along with the
   ID number, is stored in the Client struct.
*/

// A map from the ID numbers to the Client structs
set<SocketConnection*> clients;

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
void remove_client(SocketConnection *sc) {
    s->closeConnection(sc);
    clients.erase(sc);
    delete sc;
}

int main() {
    verify();

    srand((unsigned int)time(NULL));

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

	Game* game = new CTFGame(HBMap("map.hbm"));

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
                // kill server
                // should probably also deal with any clients
                close(sockfd);
                exit(0);
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

            clients.insert(sc);

            printf("Client added to clients map\n");
        }

		// Loop through all clients, and see if there are
		// any messages to be received.
        for(auto it = clients.begin();
                it != clients.end();) {
            auto next_it = it;
            ++next_it;

            SocketConnection* sc = *it;

			// First, we see when the last time we received
			// a packet from them is. If it was too long ago,
			// we disconnect. The contract between the client
			// and server requires that the client send a message
			// at least once every TIMEOUT seconds to demonstrate
			// that it is still present.
            if(sc->lastTimeReceived < time(NULL) - TIMEOUT) {
				WritePacket wp(STC_DISCONNECT, 0);
				sc->send_packet(wp);

                game->removePlayer(sc);
                remove_client(sc);
                printf("Client timed out, currently %zu clients\n",
                        clients.size());
            }

			// If the client is still here, then we read any packets
			// that we have from it. Most packet types are dealt
			// with by game. The only packets we have to handle here
			// are the CTS_CONNECT and CTS_DISCONNECT messages.
            else {
                ReadPacket* rp;
                while((rp = sc->receive_packet()) != NULL) {
                    // Connect
                    if(rp->message_type == CTS_CONNECT) {
                        printf("Received connect message\n");

                        if(game->addPlayer(sc))
                            printf("Player added to game; currently %zu clients\n",
                                     clients.size());
                    }

                    // Disconnect
                    else if(rp->message_type == CTS_DISCONNECT) {
                        game->removePlayer(sc);
                        remove_client(sc);
                        printf("Player disconnected, currently %zu clients\n",
											clients.size());
                    }

					// Give packet to game for processing
					// if the packet is of any other type.
                    else if (rp->message_type == CTS_USER_STATE &&
                             rp->packet_number >= sc->largestPacketNum) {
                        game->process_packet(sc, rp);
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
        game->update(dt);

		// Send the world state to all clients
		worldWritePacket.reset();
		game->getWorld().writeToPacket(&worldWritePacket);
		for(auto iter = clients.begin();
		        iter != clients.end(); ++iter) {
		    SocketConnection *sc = *iter;
		    int playerID = game->getObjectIDOf(sc);
		    if (playerID != -1) {
				worldWritePacket.write_int(game->getObjectIDOf(sc));
				sc->send_packet(worldWritePacket);
				worldWritePacket.backup(sizeof(int));
			}
		}

		// Sleep a little while before the
		// next iteration.
        usleep(TIMESTEP_MICROSECONDS);
    }
}
