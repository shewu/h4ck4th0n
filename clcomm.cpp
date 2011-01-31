#include "clcomm.h"
#include "hack.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>
#include <cstring>
using namespace std;

#define MYPORT "3490"
#define BACKLOG 10

vector<ClientCommunicator> sockets;

void listen_for_clients() {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, MYPORT, &hints, &res);

	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, BACKLOG);

	int next_id = 0;

	while(true) {
		sockaddr_storage their_addr;
		socklen_t addr_size = sizeof their_addr;
		int new_fd = accept(sockfd, (sockaddr *)&their_addr, &addr_size);

		ClientCommunicator clcomm;
		clcomm.sock = Socket(new_fd);
		clcomm.object_id = (next_id++);
		sockets.push_back(clcomm);
	}
}

