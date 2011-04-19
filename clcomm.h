#include "hack.h"
#include "socket.h"

class ClientCommunicator {
	public:
		SocketConnection* sc;
		int id;
		int object_id;
		char key_pressed;
		float angle;
		double spawnTime;
		int team;
};
