#include "hack.h"
#include <map>
#include <netinet/in.h>
#include <cstdio>
using namespace std;

bool World::sendObjects(Socket socket) {
	int a = htonl(objects.size());
	if(!socket.send((char*)(&a), 4))
		return false;
	
	int s = objects.size();
	for(map<int, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		if (!it->second.send(socket))
			return false;
		s--;
	}
	return true;
}

bool World::receiveObjects(Socket socket) {
	int a;
	socket.receive((char*)(&a), 4);
	int numObjects = ntohl(a);
	objects.clear();
	for(int i = 0; i < numObjects; i++) {
		Object o;
		if(!o.receive(socket))
			return false;
		objects[o.id] = o;
	}
	return true;
}

