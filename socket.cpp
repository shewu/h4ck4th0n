#include "hack.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <cstdio>

bool Socket::send(char* stuff, int size) {
	int pos = 0;
	while (pos < size) {
		int r = write(socket, stuff+pos, size-pos);
		if (r <= 0) return false;
		pos += r;
	}
	return true;
}

bool Socket::receive(char* stuff, int size) {
	int pos = 0;
	while (pos < size) {
		printf("pos = %d\n", pos);
		int r = read(socket, stuff+pos, size-pos);
		if (r <= 0) return false;
		pos += r;
	}
	return true;
}

bool Socket::hasRemaining() {
	char c;
	int val = recv(socket, &c, 1, MSG_DONTWAIT|MSG_PEEK);
	return (val != -1 || (errno != EAGAIN && errno != EWOULDBLOCK));
}
