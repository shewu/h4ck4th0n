#include "server.h"
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

#include <sys/poll.h>

using namespace std;

#define MYPORT "55555"

//does not appear to serve any purpose?
//#define BACKLOG 10

#define TIMESTEP_MICROSECONDS 10000

map<int, Client> clients;
timeval tim;

Game game;

void verify() {
    if(FRICTION < 0.0f) {
        cout << "Your friction is negative and will cause the game to fail\n";
        exit(-1);
    }
}

void remove_client(Client cl) {
    delete cl.sc;
    clients.erase(cl.id);
}

int main() {
    verify();

    srand((unsigned int)time(NULL));

    gettimeofday(&tim, NULL);

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
    Socket s(sockfd);
    s.listen();

    while(true) {
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

        sockaddr_storage their_addr;

        SocketConnection *sc = s.receiveConnection();
        if(sc != NULL) {
            if(clients.size() >= MAX_CLIENTS) {
                delete sc;
                continue;
            }
            
            printf("Connection made\n");

            Client cl;

            do { cl.id = rand(); }
            while(clients.count(cl.id));

            cl.sc = sc;

            clients[cl.id] = cl;

            printf("Client added to clients map\n");
        }

        for(map<int, Client>::iterator it = clients.begin();
                it != clients.end();) {
            map<int, Client>::iterator next_it = it;
            ++next_it;

            Client cl = it->second;

            if(cl.sc->lastTimeAcknowledged < time(NULL) - TIMEOUT) {
                char q = 0;
                cl.sc->add(&q, 1);
                cl.sc->send();
                game.remove_player(cl.id);
                remove_client(cl);
                printf("Client timed out, currently %zu clients\n",
                        clients.size());
            }
            else {
                int n;
                char buf[MAX_PACKET_LENGTH_CLIENT_TO_SERVER];
                while((n = cl.sc->receive(buf, 
                                MAX_PACKET_LENGTH_CLIENT_TO_SERVER)) >= 0) {
                    
                    //Message of length 0 indicated connection
                    if(n == 0) {
                        printf("Received connect message\n");

                        game.add_player(cl);

                        printf("Player added to game; currently %zu clients\n",
                                clients.size());
                    }

                    //One byte of 0 indictates disconnect
                    else if(n == 1 && buf[0] == 0) {
                        game.remove_player(cl.id);
                        remove_client(cl);
                        printf("Player disconnected, currently %zu clients\n",
                                clients.size());
                    }

                    else {
                        game.process_packet(cl.id, buf, n);
                    }
                }
            }

            it = next_it;
        }

        timeval tim2;
        gettimeofday(&tim2, NULL);
        float dt = (float)(tim2.tv_sec - tim.tv_sec) +
                (float)(tim2.tv_usec - tim.tv_usec) * 1.0e-6f;
        tim = tim2;

        game.update(dt);

        game.send_world();

        usleep(TIMESTEP_MICROSECONDS);
    }

}

