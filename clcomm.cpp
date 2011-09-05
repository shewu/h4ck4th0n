#include "clcomm.h"
#include "hack.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <cstring>
#include <sys/time.h>
#include <ctime>
#include <cstdlib>
#include <SDL/SDL.h>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <iostream>

using namespace std;

#define MYPORT "55555"
#define BACKLOG 10

map<int, ClientCommunicator> clients;

timeval tim;

enum {
	RED,
	BLU,
};

int main() {
	if(FRICTION < 0.0f) {
		cout << "your friction is negative and will cause the game to fail\n";
		exit(-1);
	}
	srand((unsigned int)time(NULL));
	
	gettimeofday(&tim, NULL);
	
	World world;
	
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL, MYPORT, &hints, &res);
	
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	bind(sockfd, res->ai_addr, res->ai_addrlen);
	Socket s(sockfd);
	s.listen();
	
	int teamCount[2];
	while (true) {
		sockaddr_storage their_addr;
		socklen_t addr_size = sizeof their_addr;
		SocketConnection* sc = s.receiveConnection();
		if (sc != NULL) {
			if (clients.size() == MAX_CLIENTS) {
				delete sc;
				continue;
			}
			
			printf("Connection made\n");
			
			ClientCommunicator clcomm;
			
			do {
				clcomm.id = rand();
			} while (clients.count(clcomm.id));
			
			if (teamCount[RED] == teamCount[BLU]) {
				clcomm.team = rand() % 2;
			} else {
				clcomm.team = teamCount[RED] > teamCount[BLU];
			}
			teamCount[clcomm.team]++;
			clcomm.sc = sc;
			clcomm.object_id = -1;
			clcomm.angle = rand()/float(RAND_MAX)*2*M_PI;
			clcomm.spawnTime = (double)tim.tv_sec + (double)tim.tv_usec*1.0e-6+SPAWN_TIME;
			
			clcomm.key_pressed = 0;
			
			clients[clcomm.id] = clcomm;
			
			printf("Client connected\n");
		}
		
		for (map<int, ClientCommunicator>::iterator it = clients.begin(); it != clients.end();) {
			world.sendObjects(it->second.sc, it->second.object_id);
			map<int, ClientCommunicator>::iterator nit = it;
			nit++;
			
			while (it->second.object_id == -1 && (double)tim.tv_sec + (double)tim.tv_usec*1.0e-6 >= it->second.spawnTime) {
				it->second.object_id = world.spawn(2*it->second.team, it->second.id, -1);
				if (it->second.object_id == -1) it->second.spawnTime += SPAWN_TRY;
			}
			
			char data[5];
			int n;
			
			if (it->second.sc->lastTimeAcknowledged < time(NULL)-TIMEOUT) {
				char q = 0;
				it->second.sc->add(&q, 1);
				it->second.sc->send();
				if (it->second.object_id != -1) {
					if (!world.objects[it->second.object_id].dead) world.objects.erase(it->second.object_id);
					else world.objects[it->second.object_id].player = -2;
				}
				delete it->second.sc;
				clients.erase(it);
				printf("Client timed out size = %zu\n", clients.size());
			}
			else while ((n = it->second.sc->receive(data, 5)) >= 0) {
				if (n == 0) {
					int angle = htonl(*reinterpret_cast<int*>(&it->second.angle));
					it->second.sc->add((char*)(&angle), 4);
					it->second.sc->send();
				}
				if (n == 1 && data[0] == 0) {
					if (it->second.object_id != -1) {
						if (!world.objects[it->second.object_id].dead) world.objects.erase(it->second.object_id);
						else world.objects[it->second.object_id].player = -2;
					}
					delete it->second.sc;
					clients.erase(it);
					printf("Client disconnected size = %zu\n", clients.size());
					break;
				}
				if (n != 5) continue;
				int angle = ntohl(*((int*)data));
				it->second.angle = *reinterpret_cast<float*>(&angle);
				it->second.key_pressed = data[4];
			}
			
			it = nit;
		}
		
		timeval tim2;
		gettimeofday(&tim2, NULL);
		float dt = (float)(tim2.tv_sec - tim.tv_sec) + (float)(tim2.tv_usec - tim.tv_usec) * 1.0e-6f;
		tim = tim2;
		
		for (map<int, ClientCommunicator>::iterator it = clients.begin(); it != clients.end(); it++) {
			Vector2D acceleration = Vector2D(0.0f, 0.0f);
			int value = 0;
			if (it->second.key_pressed & 1) {
				acceleration += Vector2D(cos(it->second.angle), sin(it->second.angle));
				value += 1;
			}
			if (it->second.key_pressed & 2) {
				acceleration += Vector2D(-cos(it->second.angle), -sin(it->second.angle));
				value -= 1;
			}
			if (it->second.key_pressed & 4) {
				acceleration += Vector2D(sin(it->second.angle), -cos(it->second.angle));
				value += 2;
			}
			if (it->second.key_pressed & 8) {
				acceleration += Vector2D(-sin(it->second.angle), cos(it->second.angle));
				value -= 2;
			}
			if (value != 0) {
				acceleration = acceleration.getNormalVector()*KEYPRESS_ACCELERATION;
				if (it->second.object_id != -1 && !world.objects[it->second.object_id].dead) world.objects[it->second.object_id].v += acceleration*dt;
			}
		}
		
		for (map<int, Object>::iterator it = world.objects.begin(); it != world.objects.end();) {
			map<int, Object>::iterator nit = it;
			nit++;
			if (!it->second.dead) it->second.v -= it->second.v*FRICTION*dt;
			else if (it->second.spawny == 0) {
				it->second.spawny = 1;
				it->second.spawnTime = (double)tim.tv_sec + (double)tim.tv_usec*1.0e-6+SPAWN_TIME;
			}
			else if (it->second.spawny == 1) {
				if (it->second.player == -2) it->second.spawny = 2;
				else while (it->second.spawny == 1 && (double)tim.tv_sec + (double)tim.tv_usec*1.0e-6 > it->second.spawnTime) {
					int newo = world.spawn(it->second.spawnl, it->second.player, it->second.flag);
					if (newo == -1) it->second.spawnTime += SPAWN_TRY;
					else {
						it->second.spawny = 2;
						if (it->second.player != -1) {
							clients[it->second.player].object_id = newo;
							it->second.player = -1;
						}
					}
				}
			}
			else if (it->second.stopped) world.objects.erase(it);
			it = nit;
		}
		
		world.doSimulation(dt);
		usleep(10000);
	}
}
