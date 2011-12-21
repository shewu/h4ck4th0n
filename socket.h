#ifndef SOCKET_H
#define SOCKET_H

#include "constants.h"
#include "packet.h"

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <map>

/* This header file contains two important classes, Socket
   and SocketConnection. Socket is a class which handles
   general networking; a SocketConnection is a connection
   with a particular address.

   - The server should call Socket::listen_for_client() once
     and then use Socket::receiveConnection() to obtain
	 SocketConnection instances for all connecting clients.

   - The client should call Socket::connect_to_server()
     to obtain a SocketConnection instance which communicates
	 to the server. The client should only need one
	 SocketConnection instance.

   To send messages, use the ReadPacket and WritePacket
   classes. SocketConnection::send_packet takes a WritePacket
   as an argument, and SocketConnection::receive_packet
   returns a ReadPacket (or NULL if no packet is received).
   See packet.h.

   Also see network_protocol.h for specification of the
   messaging protocol.
*/

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
