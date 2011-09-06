#ifndef SERVER_H
#define SERVER_H

#include "hack.h"
#include "socket.h"

class Client {
	public:
		SocketConnection* sc;
		int id;
};


#endif
