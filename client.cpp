#include "hack.h"
#include <iostream>
#include "render.h"
using namespace std;

int main(int argc, char* argv[])
{
	Socket sock;
	World w;
	for (;;) {
		do {
			render();
			w.recieveObjects(sock);
		} while (sock.hasRemaining());
	}
}
