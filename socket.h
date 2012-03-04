#ifndef SOCKET_H
#define SOCKET_H

#include "constants.h"
#include "packet.h"

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <map>
#include <queue>

/* This header file contains two important classes, Socket
   and SocketConnection. Socket is a class which handles
   general networking; a SocketConnection is a connection
   with a particular address.

   - The server should call Socket::listen_for_client() once
     and then use Socket::receiveConnection() to obtain
	 SocketConnection instances for all connecting clients.

   - The client should call Socket::connect()
     to obtain a SocketConnection instance which communicates
	 to the server. The client should only need one
	 SocketConnection instance.

   Note that the constructor for Socket takes a socket
   file descriptor as an argument. Currently, the code
   for making this socket can be found in server.cpp
   for the server and in GameViewController.cpp for
   for the client.

   To send messages, use the ReadPacket and WritePacket
   classes. SocketConnection::send_packet takes a WritePacket
   as an argument, and SocketConnection::receive_packet
   returns a ReadPacket (or NULL if no packet is received).
   See packet.h.

   Also see NetworkProtocol.h for specification of the
   messaging protocol.
*/

class SocketConnection;

class Socket
{
	public:
		Socket(int sock);

		void listen_for_client();
		class SocketConnection* receiveConnection();
        void closeConnection(SocketConnection *sc);

		class SocketConnection* connect(sockaddr *addr, socklen_t addrlen);
        void end_connection();

		void recv_all();
		
    private:
        int socket;
		bool listening;
        std::map<std::pair<std::string, int>, SocketConnection*> connections;
		std::queue<SocketConnection*> new_connections;
};

class SocketConnection {
	public:
        int socket;
		struct sockaddr *addr;
		socklen_t addrlen;
		int my_id, their_id;

		SocketConnection(int socket, struct sockaddr *addr, socklen_t addrlen,
		                 int my_id, int their_id);
		void send_packet(WritePacket*);
		ReadPacket* receive_packet();
		void recv_data(char *buf, int length);
		~SocketConnection();

		int lastTimeReceived;

    private:
        int num_sent;
		std::queue<ReadPacket*> read_packets;

};

#endif
