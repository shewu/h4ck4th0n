#ifndef SOCKET_H
#define SOCKET_H

#include "constants.h"
#include "packet.h"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <map>

class SocketConnection;

class Socket
{
	public:
		Socket(int sock);

		void listen();
		SocketConnection* receiveConnection();
        void closeConnection();

		SocketConnection* connect(sockaddr addr, socklen_t addrlen);
        void disconnect();
		
    private:
        int socket;
		bool listening, connected;
        map<int, SocketConnection*> connections;
};

class SocketConnection {
	public:
        int socket;

		SocketConnection(int socket);
		void send(WritePacket*);
		ReadPacket* receive();
		~SocketConnection();

		int lastTimeReceived;

    private:
        int num_sent;
};

#endif
