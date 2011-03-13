#include "hack.h"

class ClientCommunicator {
	public:
		Socket sock;
		int id;
		int object_id;
		bool key_pressed[4];
		float angle;
		bool waiting;
		char buf[4];
		int pos;
		double spawnTime;
};
