#include "socket.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

Socket::Socket(int sock) {
	socket = sock;
	qf = qb = 0;
	listening = false;
}

void Socket::receivePackets() {
	char stuff[MAXPACKET];
	sockaddr addr;
	socklen_t addrlen;
	int a;
	while ((addrlen = sizeof(sockaddr)) && (a = recvfrom(socket, stuff, MAXPACKET, MSG_DONTWAIT, &addr, &addrlen)) >= 0) {
		if (a < 20) continue;
		string s((char*)(&addr), addrlen);
		if (ntohl(*(int*)(stuff)) != PROTMAGIC) continue;
		int v = ntohl(*(int*)(stuff+4));
		if (connections.count(pair<string, int>(s, v))) {
			SocketConnection* c = connections[pair<string, int>(s, v)];
			int p = ntohl(*(int*)(stuff+8));
			if (p < c->lastReceived) continue;
			c->lastReceived = ntohl(*(int*)(stuff+8));
			if (ntohl(*(int*)(stuff+12)) > c->lastTimeReceived) c->lastTimeReceived = ntohl(*(int*)(stuff+12));
			if (ntohl(*(int*)(stuff+16)) > c->lastTimeAcknowledged) c->lastTimeAcknowledged = (ntohl(*(int*)(stuff+16)) < time(NULL) ? ntohl(*(int*)(stuff+16)) : time(NULL));
			if (c->qb-c->qf < MAXQUEUE) c->queue[(c->qb++)%MAXQUEUE] = string(stuff+20, a-20);
		}
		else if (a == 20 && listening) {
			if (qb-qf < MAXQUEUE) connections[pair<string, int>(s, v)] = queue[(qb++)%MAXQUEUE] = new SocketConnection(this, addr, addrlen, v);
			
			SocketConnection* c = connections[pair<string, int>(s, v)];
			if (ntohl(*(int*)(stuff+8)) < c->lastReceived) continue;
			c->lastReceived = ntohl(*(int*)(stuff+8));
			if (ntohl(*(int*)(stuff+12)) > c->lastTimeReceived) c->lastTimeReceived = ntohl(*(int*)(stuff+12));
			if (ntohl(*(int*)(stuff+16)) > c->lastTimeAcknowledged) c->lastTimeAcknowledged = (ntohl(*(int*)(stuff+16)) < time(NULL) ? ntohl(*(int*)(stuff+16)) : time(NULL));
			if (c->qb-c->qf < MAXQUEUE) c->queue[(c->qb++)%MAXQUEUE] = "";
		}
	}
}

void Socket::listen() {
	listening = true;
}

SocketConnection* Socket::receiveConnection() {
	receivePackets();
	if (qf < qb) {
		SocketConnection* sc = queue[qf++];
		if (qf >= MAXQUEUE) {
			qf -= MAXQUEUE;
			qb -= MAXQUEUE;
		}
		return sc;
	}
	else return NULL;
}

SocketConnection* Socket::connect(sockaddr addr, socklen_t addrlen) {
	string s((char*)(&addr), addrlen);
	int a = rand();
	return connections[pair<string, int>(s, a)] = new SocketConnection(this, addr, addrlen, a);
}

SocketConnection::SocketConnection(Socket* s, sockaddr addr,  socklen_t addrlen, int v) {
	sock = s;
	this->addr = addr;
	this->addrlen = addrlen;
	this->v = v;
	qf = qb = 0;
	
	packetnum = 0;
	lastReceived = -1;
	lastTimeReceived = -1;
	lastTimeAcknowledged = time(NULL);
}

void SocketConnection::add(char* stuff, int size) {
	toSend += string(stuff, size);
}

void SocketConnection::send() {
	char buffer[toSend.size()+20];
	*((int*)(buffer)) = htonl(PROTMAGIC);
	*((int*)(buffer+4)) = htonl(v);
	*((int*)(buffer+8)) = htonl(packetnum++);
	*((int*)(buffer+12)) = htonl(time(NULL));
	*((int*)(buffer+16)) = htonl(lastTimeReceived);
	memcpy(buffer+20, &toSend[0], toSend.size());
	sendto(sock->socket, buffer, toSend.size()+20, 0, &addr, addrlen);
	toSend.clear();
}

int SocketConnection::receive(char* stuff, int size) {
	sock->receivePackets();
	if (qf >= qb) return -1;
	int ss = (size < queue[qf].size() ? size : queue[qf].size());
	memcpy(stuff, &queue[qf++][0], ss);
	if (qf >= MAXQUEUE) {
		qf -= MAXQUEUE;
		qb -= MAXQUEUE;
	}
	return ss;
}

SocketConnection::~SocketConnection() {
	sock->connections.erase(pair<string, int>(string((char*)(&addr), addrlen), v));
}
