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
#include <SDL/SDL.h>
#include <cstdio>
#include <cmath>
using namespace std;

#define MYPORT "55555"
#define BACKLOG 10

vector<ClientCommunicator> clients;

timeval tim;

int main() {
	srand((unsigned int)time(NULL));
	
	gettimeofday(&tim, NULL);
	
	World world;
	
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL, MYPORT, &hints, &res);
	
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	listen(sockfd, BACKLOG);
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	
	while(true) {
		sockaddr_storage their_addr;
		socklen_t addr_size = sizeof their_addr;
		int new_fd = accept(sockfd, (sockaddr *)&their_addr, &addr_size);
		if(new_fd != -1) {
			printf("Connection made\n");
			
			ClientCommunicator clcomm;
			clcomm.sock = Socket(new_fd);
			do {
				clcomm.object_id = rand();
			} while (world.objects.count(clcomm.object_id));
			
			clcomm.angle = rand()/float(RAND_MAX)*2*M_PI;
			
			Object o;
			o.v = Vector2D(0.0f, 0.0f);
			o.mass = 1.0f;
			o.rad = 1.0f;
			o.color = Color(0, 255, 0);
			o.h = 1.0f;
			o.id = clcomm.object_id;
			float min_x = MIN_X + o.rad;
			float max_x = MAX_X - o.rad;
			float min_y = MIN_Y + o.rad;
			float max_y = MAX_Y - o.rad;
			while(true) {
				o.p.x = rand()/float(RAND_MAX)*(max_x - min_x) + min_x;
				o.p.y = rand()/float(RAND_MAX)*(max_y - min_y) + min_y;
				bool fail = false;
				for(map<int, Object>::iterator it = world.objects.begin(); it != world.objects.end(); it++) {
					Vector2D v = it->second.p - o.p;
					if((it->second.rad + o.rad) * (it->second.rad + o.rad) > v.x * v.x + v.y * v.y) {
						fail = true;
						break;
					}
				}
				if(!fail)
					break;
			}
			
			world.objects.insert(pair<int, Object>(o.id, o));
			clcomm.waiting = false;
			for (int i = 0; i < 4; i++) clcomm.key_pressed[i] = 0;
			clients.push_back(clcomm);
			
			int id = htonl(o.id);
			clients[clients.size() - 1].sock.send((char*)(&id), 4);
			int angle = htonl(*reinterpret_cast<int*>(&clcomm.angle));
			clients[clients.size() - 1].sock.send((char*)(&angle), 4);
			
			printf("Object created\n");
		}
		
		for(int i = 0; i < clients.size(); i++) {
			world.sendObjects(clients[i].sock);
			while (clients[i].sock.hasRemaining()) {
				char data;
				if(!clients[i].sock.receive(&data, 1)) {
					world.objects.erase(clients[i].object_id);
					for(int j = i; j < clients.size() - 1; j++)
						clients[j] = clients[j+1];
					clients.pop_back();
					printf("Client disconnected size = %d\n", clients.size());
					i--;
					break;
				}
				if (clients[i].waiting) {
					clients[i].buf[clients[i].pos++] = data;
					if (clients[i].pos == 4) {
						clients[i].waiting = false;
						int angle = ntohl(*((int*)clients[i].buf));
						clients[i].angle = *reinterpret_cast<float*>(&angle);
					}
				}
				else if (data == 8) {
					clients[i].waiting = true;
					clients[i].pos = 0;
				}
				else clients[i].key_pressed[(unsigned char)data&3] = ((data&4) == 0);
			}
		}

		timeval tim2;
		gettimeofday(&tim2, NULL);
		float dt = (float)(tim2.tv_sec - tim.tv_sec) + (float)(tim2.tv_usec - tim.tv_usec) * 1.0e-6f;
		tim = tim2;

		for(vector<ClientCommunicator>::iterator it = clients.begin(); it != clients.end(); ++it) {
			Vector2D acceleration = Vector2D(0.0f, 0.0f);
			int value = 0;
			if(it->key_pressed[0]) {
				acceleration += Vector2D(-cos(it->angle), -sin(it->angle));
				value += 1;
			}
			if(it->key_pressed[1]) {
				acceleration += Vector2D(cos(it->angle), sin(it->angle));
				value -= 1;
			}
			if(it->key_pressed[2]) {
				acceleration += Vector2D(-sin(it->angle), cos(it->angle));
				value += 2;
			}
			if(it->key_pressed[3]) {
				acceleration += Vector2D(sin(it->angle), -cos(it->angle));
				value -= 2;
			}
			if(value != 0)
				acceleration = acceleration.getNormalVector()*KEYPRESS_ACCELERATION;
			acceleration -= world.objects[it->object_id].v*0.2;
			world.objects[it->object_id].v += acceleration*dt;
		}

		world.doSimulation(dt);
		SDL_Delay(10);
	}
}

