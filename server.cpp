#include "server.h"
#include "packet.h"
#include "game.h"
#include "hack.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>

#include <cstring>
#include <sys/time.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>

#include <unistd.h>
#include <errno.h>

#include <sys/poll.h>

using namespace std;

#define MYPORT "55555"

#define TIMESTEP_MICROSECONDS 10000

/* Every connecting client is randomly assigned
   a unique client ID number. The SocketConnection
   corresponding to that client, along with the
   ID number, is stored in the Client struct.
*/

// A map from the ID numbers to the Client structs
map<int, Client> clients;

// Global variable keeping track of time
timeval tim;

// Deals with the game state, i.e., scores, teams,
// world, etc.
Game game;

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
void remove_client(Client cl) {
    s->closeConnection(cl.sc);
    clients.erase(cl.id);
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

            Client cl;

            do { cl.id = rand(); }
            while(clients.count(cl.id));

            cl.sc = sc;

            clients[cl.id] = cl;

            printf("Client added to clients map\n");
        }

		// Loop through all clients, and see if there are
		// any messages to be received.
        for(map<int, Client>::iterator it = clients.begin();
                it != clients.end();) {
            map<int, Client>::iterator next_it = it;
            ++next_it;

            Client cl = it->second;

			// First, we see when the last time we received
			// a packet from them is. If it was too long ago,
			// we disconnect. The contract between the client
			// and server requires that the client send a message
			// at least once every TIMEOUT seconds to demonstrate
			// that it is still present.
            if(cl.sc->lastTimeReceived < time(NULL) - TIMEOUT) {
				WritePacket *wp = new WritePacket(STC_DISCONNECT, 0);
				cl.sc->send_packet(wp);
				delete wp;

                game.remove_player(cl.id);
                remove_client(cl);
                printf("Client timed out, currently %zu clients\n",
                        clients.size());
            }

			// If the client is still here, then we read any packets
			// that we have from it. Most packet types are dealt
			// with by game. The only packets we have to handle here
			// are the CTS_CONNECT and CTS_DISCONNECT messages.
            else {
                ReadPacket* rp;
                while((rp = cl.sc->receive_packet()) != NULL) {

                    // Connect
                    if(rp->message_type == CTS_CONNECT) {
                        printf("Received connect message\n");

                        if(game.add_player(cl))
                            printf("Player added to game; currently %zu clients\n",
                                     clients.size());
                    }

                    // Disconnect
                    else if(rp->message_type == CTS_DISCONNECT) {
                        game.remove_player(cl.id);
                        remove_client(cl);
                        printf("Player disconnected, currently %zu clients\n",
											clients.size());
                    }

					// Give packet to game for processing
					// if the packet is of any other type.
                    else {
                        game.process_packet(cl.id, rp);
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
        game.update(dt);

		// Send the world state to all clients
        game.send_world();

		// Sleep a little while before the
		// next iteration.
        usleep(TIMESTEP_MICROSECONDS);
    }

}
