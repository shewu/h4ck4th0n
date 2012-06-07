#include "Socket.h"
#include "Hack.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

using namespace std;

#define BACKLOG 10
#define SERVER_ID 0

Socket::Socket(int sock) {
    socket = sock;
    listening = false;
}

void Socket::listen_for_client() {
    listening = true;
}

SocketConnection* Socket::receiveConnection() {
	if(new_connections.size() == 0)
		return NULL;
	SocketConnection *sc = new_connections.front();
	new_connections.pop();
    return sc;
}

void Socket::closeConnection(SocketConnection *sc) {
    connections.erase(pair<string, int>(string((char*)sc->addr, sc->addrlen), sc->their_id));
    delete sc;
}

SocketConnection* Socket::connect(sockaddr *addr, socklen_t addrlen) {
	int id = rand();
	SocketConnection* sc = new SocketConnection(socket, addr, addrlen, id, SERVER_ID);
	string s((char *)addr, addrlen);
	connections[pair<string,int>(s,SERVER_ID)] = sc;
	return sc;
}

void Socket::end_connection() {
	close(socket);
}

void Socket::recv_all() {
	static char buf[MAXPACKET];
	while(true) {
		sockaddr addr;
		socklen_t addrlen = sizeof(struct sockaddr);
		int len = recvfrom(socket, buf, MAXPACKET, MSG_DONTWAIT, &addr, &addrlen);
		if(len < 8)
			break;
		int magic;
		memcpy((void*)&magic, (void*)buf, 4);
		magic = ntohl(magic);
		if (magic != MESSAGE_HEADER_INIT) continue;
		string s = string((char *)&addr, addrlen);
		int id;
		memcpy((void*)&id, (void*)(buf + 4), 4);
		id = ntohl(id);
		pair<string,int> s_id(s, id);

		SocketConnection *sc;
		if(connections.count(s_id) == 0) {
			if(!listening)
				continue;
			sc = new SocketConnection(socket, &addr, addrlen, SERVER_ID, id);
			new_connections.push(sc);
			connections[s_id] = sc;
		} else {
			sc = connections[s_id];
		}

		sc->recv_data(buf + 8, len - 8);
	}
}
