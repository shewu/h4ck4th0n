#include "hack.h"
#include <netinet/in.h>

bool Object::send(Socket socket) {
	char *buf = new char[35];
	*((long *)buf) = htonl(reinterpret_cast<long>(p.x));
	*((long *)(buf + 4)) = htonl(reinterpret_cast<long>(p.y));
	*((long *)(buf + 8)) = htonl(reinterpret_cast<long>(v.x));
	*((long *)(buf + 12)) = htonl(reinterpret_cast<long>(v.y));
	*((long *)(buf + 16)) = htonl(reinterpret_cast<long>(mass));
	*((long *)(buf + 20)) = htonl(reinterpret_cast<long>(rad));
	*(buf + 24) = color.r;
	*(buf + 25) = color.g;
	*(buf + 26) = color.b;
	*((long *)(buf + 27)) = htonl(*reinterpret_cast<long *>(&h));
	*((long *)(buf + 31)) = htonl(id);
	return socket.send(buf, 35);
}

bool Object::receive(Socket socket) {
	char *buf = new char[35];
	if(!socket.receive(buf, 35))
		return false;
	p.x = reinterpret_cast<float>ntohl(*((long *)(buf)));
	p.y = reinterpret_cast<float>ntohl(*((long *)(buf + 4)));
	v.x = reinterpret_cast<float>ntohl(*((long *)(buf + 8)));
	v.y = reinterpret_cast<float>ntohl(*((long *)(buf + 12)));
	mass = reinterpret_cast<float>ntohl(*((long *)(buf + 16)));
	rad = reinterpret_cast<float>ntohl(*((long *)(buf + 20)));
	color.r = *(buf + 24);
	color.g = *(buf + 25);
	color.b = *(buf + 26);
	h = reinterpret_cast<float>nothl(*((long *)(buf + 27)));
	id = ntohl(*((long *)(buf + 31)));
	return true;
}
