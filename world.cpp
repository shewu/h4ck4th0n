#include "hack.h"
#include <map>
#include <netinet/in.h>
#include <cstdio>
using namespace std;

bool World::sendObjects(Socket socket) {
	int a = htonl(objects.size());
	if (!socket.send((char*)(&a), 4))
		return false;
	
	int s = objects.size();
	for(map<int, Object>::iterator it = objects.begin(); it != objects.end(); it++) {
		if (!it->second.send(socket))
			return false;
		s--;
	}
	
	int b = htonl(sounds.size());
	if (!socket.send((char*)(&b), 4))
		return false;
	for(vector<pair<char, Vector2D> >::iterator it = sounds.begin(); it != sounds.end(); it++) {
		char buf[9];
		buf[0] = it->first;
		*((int*)(buf+1)) = htonl(*reinterpret_cast<int*>(&it->second.x));
		*((int*)(buf+5)) = htonl(*reinterpret_cast<int*>(&it->second.y));
		if (!socket.send(buf, 9))
			return false;
	}
	return true;
}

bool World::receiveObjects(Socket socket) {
	int a;
	socket.receive((char*)(&a), 4);
	int numObjects = ntohl(a);
	objects.clear();
	for (int i = 0; i < numObjects; i++) {
		Object o;
		if(!o.receive(socket))
			return false;
		objects[o.id] = o;
	}
	
	int b;
	socket.receive((char*)(&b), 4);
	int numSounds = ntohl(b);
	sounds.clear();
	sounds.reserve(numSounds);
	for (int i = 0; i < numSounds; i++) {
		char buf[9];
		if (!socket.receive(buf, 9))
			return false;
		*((int*)(buf+1)) = ntohl(*((int*)(buf+1)));
		*((int*)(buf+5)) = ntohl(*((int*)(buf+5)));
		sounds.push_back(pair<char, Vector2D>(buf[0], Vector2D(*reinterpret_cast<float*>(buf+1), *reinterpret_cast<float*>(buf+5))));
	}
	return true;
}

