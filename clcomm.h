#include "hack.h"

class ClientCommunicator {
	public:
		Socket sock;
		int object_id;
		bool key_pressed[256];
		float angle;
		bool waiting;
		char buf[4];
		int pos;
};
