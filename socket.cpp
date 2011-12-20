#include "socket.h"
#include "hack.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>

using namespace std;

#define BACKLOG 10

Socket::Socket(int sock) {
    socket = sock;
    listening = false;
    connected = false;
}

void Socket::listen_for_client() {
    if(connected || listening)
        return;
    listen(socket, BACKLOG);
    fcntl(socket, F_SETFL, O_NONBLOCK);
    listening = true;
}

SocketConnection* Socket::receiveConnection() {
    if(!listening || connections.size() >= MAX_CLIENTS)
        return NULL;
    struct sockaddr addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    int new_socket = accept(socket, &addr, &addr_len);
    if(new_socket == -1) {
		if(errno != EWOULDBLOCK) {
			printf("server accept error: error = %d\n", errno);
		}
        return NULL;
	}
    SocketConnection *sc = new SocketConnection(new_socket);
    connections[new_socket] = sc;
    return sc;
}

void Socket::closeConnection(SocketConnection *sc) {
    connections.erase(sc->socket);
    delete sc;
}

SocketConnection* Socket::connect_to_server(sockaddr *addr, socklen_t addrlen) {
    if(connected || listening)
        return NULL;
    int err = connect(socket, addr, addrlen);
	printf("output = %d\n", err);
    SocketConnection* sc = new SocketConnection(socket);
    connections[socket] = sc;
    connected = true;
    return sc;
}

void Socket::end_connection() {
    if(connected) {
        close(socket);
        connected = false;
    }
}
