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

		void listen_for_client();
		SocketConnection* receiveConnection();
        void closeConnection(SocketConnection *sc);

		SocketConnection* connect_to_server(sockaddr *addr, socklen_t addrlen);
        void end_connection();
		
    private:
        int socket;
		bool listening, connected;
        std::map<int, SocketConnection*> connections;
};

class SocketConnection {
	public:
        int socket;

		SocketConnection(int socket);
		void send_packet(WritePacket*);
		ReadPacket* receive_packet();
		~SocketConnection();

		int lastTimeReceived;

    private:
        int num_sent;
};

#endif
