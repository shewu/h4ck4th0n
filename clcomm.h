#include "hack.h"

class ClientCommunicator {
	public:
		Socket sock;
		int object_id;
		bool key_pressed[256];
};
