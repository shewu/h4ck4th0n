#include "clcomm.h"
#include "hack.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <vector>
#include <cstring>
#include <sys/time.h>
#include <ctime>
#include <cstdlib>
using namespace std;

#define MYPORT "3490"
#define BACKLOG 10

vector<ClientCommunicator> clients;

timeval tim;

int main() {
	srand((unsigned int)time(NULL));

	gettimeofday(&tim, NULL);

	World *world = new World();

	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, MYPORT, &hints, &res);


	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, BACKLOG);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	int next_id = 0;

	while(true) {
		sockaddr_storage their_addr;
		socklen_t addr_size = sizeof their_addr;
		int new_fd = accept(sockfd, (sockaddr *)&their_addr, &addr_size);
		if(new_fd != -1) {
			ClientCommunicator clcomm;
			clcomm.sock = Socket(new_fd);
			clcomm.object_id = next_id;

			Object o;
			o.v = Vector2D(0.0f, 0.0f);
			o.mass = 1.0f;
			o.rad = 1.0f;
			o.color = Color(0.0f, 1.0f, 0.0f);
			o.h = 1.0f;
			o.id = next_id;
			float min_x = MIN_X + o.rad;
			float max_x = MAX_X - o.rad;
			float min_y = MIN_Y + o.rad;
			float max_y = MAX_Y - o.rad;
			while(true) {
				o.p.x = rand() * (max_x - min_x) + min_x;
				o.p.y = rand() * (max_y - min_y) + min_y;
				bool fail = false;
				for(map<int, Object>::iterator it = world->objects.begin(); it != world->objects.end(); it++) {
					Vector2D v = it->second.p - o.p;
					if((it->second.rad + o.rad) * (it->second.rad + o.rad) > v.x * v.x + v.y * v.y) {
						fail = true;
						break;
					}
				}
				if(!fail)
					break;
			}

			world->objects.insert(pair<int, Object>(next_id, o)); 
			clients.push_back(clcomm);

			next_id++;
		}

		for(vector<ClientCommunicator>::iterator it = clients.begin(); it != clients.end(); ++it) {
			world->sendObjects(it->sock);
				while(it->sock.hasRemaining()) {
				char keypress[2];
				it->sock.receive(keypress, 2);
				it->key_pressed[(unsigned char)keypress[1]] = (bool)keypress[0];
			}
		}

		timeval tim2;
		gettimeofday(&tim2, NULL);
		float dt = (float)(tim2.tv_sec - tim.tv_sec) + (float)(tim2.tv_usec - tim.tv_usec) * 1.0e-6f;
		tim = tim2;
		world->doSimulation(dt);
	}
}

