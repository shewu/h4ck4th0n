#include "hack.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

bool Socket::send(char* stuff, int size) {
	int pos = 0;
	while (pos < size) {
		int r = write(socket, stuff+pos, size-pos);
		if (r == -1) return false;
		pos -= r;
	}
	return true;
}

bool Socket::receive(char* stuff, int size) {
	int pos = 0;
	while (pos < size) {
		int r = read(socket stuff+pos, size-pos);
		if (r == -1) return false;
		pos -= r;
	}
}

bool Socket::hasRemaining() {
	char c;
	int val;
	while ((val = recv(socket, &c, 1, MSG_DONTWAIT|MSG_PEEK)) == 0);
	return (val == 1);
}
