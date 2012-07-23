#include "Logging.h"

#include <cstring>
#include <netdb.h>

#include "Packet.h"

using std::fstream;

void logRemovePlayer(fstream& f, int playerID) {
	f.put(kLoggingRemovePlayer);

	char buf[4];
	memcpy((void *) buf, (void *) &playerID, 4);
	f.write(buf, 4);
}

void logAddPlayer(fstream& f, int playerID) {
	f.put(kLoggingAddPlayer);

	char buf[4];
	memcpy((void *) buf, (void *) &playerID, 4);
	f.write(buf, 4);
}

void logProcessPacket(fstream& f, int playerID, ReadPacket* rp) {
	f.put(kLoggingProcessPacket);

	f.put(rp->message_type);

	char buf[4];
	memcpy((void *) buf, (void *) &(rp->size), 4);
	f.write(buf, 4);

	memcpy((void *) buf, (void *) &playerID, 4);
	f.write(buf, 4);

	memcpy((void *) buf, (void *) &(rp->packet_number), 4);
	f.write(buf, 4);

	f.write(rp->buf, rp->size);
}

void logDoSimulation(fstream& f, float dt) {
	f.put(kLoggingDoSimulation);
	
	char buf[4];
	memcpy((void *) buf, (void *) &dt, 4);
	f.write(buf, 4);
}

void logDone(fstream& f) {
	f.put(kLoggingDone);
}

void logRandomSeed(std::fstream& f, unsigned int seed) {
	f.put(kLoggingRandomSeed);

	char buf[4];
	memcpy((void *) buf, (void *) &seed, 4);
	f.write(buf, 4);
}
