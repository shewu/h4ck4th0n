#ifndef SERVER_H
#define SERVER_H

#include "Hack.h"
#include "Socket.h"

class Client {
	public:
		SocketConnection* sc;
		int latestPacket;
		int id;
};


#endif
