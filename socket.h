#ifndef SOCKET_H
#define SOCKET_H

#include "constants.h"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <map>

class SocketConnection;

class Socket
{
	public:
		int socket;
		Socket(int sock);
		void receivePackets();
		void listen();
		SocketConnection* receiveConnection();
		SocketConnection* connect(sockaddr addr, socklen_t addrlen);
		
		bool listening;
		SocketConnection* queue[MAXQUEUE];
		int qf, qb;
		std::map<std::pair<std::string, int>, SocketConnection*> connections;
};

class SocketConnection {
	public:
		SocketConnection(Socket* s, sockaddr addr, socklen_t addrlen, int v);
		void add(char* stuff, int size);
		void send();
		int receive(char* stuff, int size);
		~SocketConnection();
		
		sockaddr addr;
		socklen_t addrlen;
		int v;
		Socket* sock;
		std::string toSend;
		std::string queue[MAXQUEUE];
		int qf, qb;
		
		int packetnum;
		int lastReceived;
		int lastTimeReceived;
		int lastTimeAcknowledged;
};

#endif
