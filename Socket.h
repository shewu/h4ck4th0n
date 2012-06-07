#ifndef SOCKET_H
#define SOCKET_H

#include "Constants.h"
#include "Packet.h"

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

#define MESSAGE_HEADER_INIT 0x74d32f8c

class Socket
{
	public:
		/**
		 * Creates a Socket class with the given socket descriptor, which must be
		 * of type SOCK_DGRAM.
		 * @param sock The socket descriptor, which must be of type SOCK_DGRAM.
		 */
		Socket(int sock);

		/**
		 * Puts the Socket into listening mode. Should be used by a server
		 * who wants to listen for incoming connections with clients.
		 */
		void listen_for_client();

		/**
		 * Returns a new SocketConnection that was received. New sockets can only be
		 * received if in listening mode.
		 * @return a new SocketConnection, or NULL if all received connections
		 *         already been returned.
		 */
		class SocketConnection* receiveConnection();

		/**
		 * Closes the given SocketConnection.
		 * @param sc the SocketConnection instance to be closed.
		 */
        void closeConnection(SocketConnection *sc);

		/**
		 * Connects to the given address, and returns the resulting SocketConnection.
		 * @return a SocketConnection instance for the connection
		 */
		class SocketConnection* connect(sockaddr *addr, socklen_t addrlen);

		/**
		 * Closes everything
		 */
        void end_connection();

		/**
		 * Must be called periodically to process all incoming data to find new connections
		 * and pass this data to the SocketConnections.
		 */
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
		void send_packet(WritePacket&);
		ReadPacket* receive_packet();
		void recv_data(char *buf, int length);
		~SocketConnection();

		int lastTimeReceived;

    private:
        int num_sent;
		std::queue<ReadPacket*> read_packets;

};

#endif
