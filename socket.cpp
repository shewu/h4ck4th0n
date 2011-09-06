#include "socket.h"
#include "hack.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <fcntl.h>

using namespace std;

Socket::Socket(int sock) {
    socket = sock;
    listening = false;
    connected = false;
}

Socket::listen() {
    if(connected || listening)
        return;
    listen(socket, BACKLOG);
    fctnl(sockfd, F_SETFL, O_NONBLOCK);
    listening = true;
}

SocketConnection* Socket::receiveConnection() {
    if(!listening)
        return false;
    sockaddr addr;
    socklen_t addr_len;
    int new_socket = accept(socket, &addr, &addr_len);
    if(new_sockete == -1)
        return NULL;
    SocketConnection *sc = new SocketConnection(new_socket);
    connections[new_socket] = sc;
    return sc;
}

void Socket::closeConnection(SocketConnection *sc) {
    connections.erase(sc->socket);
    delete sc;
}

SocketConnection* Socket::connect(sockaddr addr, socklen_t addrlen) {
    if(connected || listening)
        return NULL;
    connect(socket, &addr, addrlen);
    SocketConnection* sc = new SocketConnection(socket);
    map[socket] = sc;
    connected = true;
    return sc;
}

SocketConnection* Socket::disconnect() {
    if(connected) {
        close(socket);
        connected = false;
    }
}
