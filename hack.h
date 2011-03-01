#ifndef HACK_H
#define HACK_H

#include <map>
#include <vector>
#include <fstream>
#include <string>
#include "constants.h"

class Socket
{
	public:
		Socket() { socket = 0; }
		Socket(int sock) { socket = sock; }
		int socket;
		bool send(char* stuff, int size);
		bool receive(char* stuff, int size);
		bool hasRemaining();
};

class Vector2D
{
	public:
		float x, y;
		Vector2D(float a = 0, float b = 0) : x(a), y(b) {}
		Vector2D operator+(const Vector2D&) const;
		Vector2D operator-(const Vector2D&) const;
		Vector2D operator-() const;
		Vector2D& operator+=(const Vector2D&);
		Vector2D& operator-=(const Vector2D&);
		Vector2D& operator*=(const float);
		bool operator==(const Vector2D&) const;
		float operator*(const Vector2D&) const;
		Vector2D operator*(const float) const;
		Vector2D getNormalVector() const;
};

Vector2D operator*(const float, const Vector2D);

class Vector3D
{
	public:
		float x, y, z, w;
		Vector3D(float a = 0, float b = 0, float c = 0) : x(a), y(b), z(c), w(0) {}
		Vector3D(Vector2D& v) : x(v.x), y(v.y), z(0), w(0) {}
		Vector3D operator+(const Vector3D&) const;
		Vector3D operator-(const Vector3D&) const;
		Vector3D operator-() const;
		Vector3D& operator+=(const Vector3D&);
		Vector3D& operator-=(const Vector3D&);
		Vector3D& operator*=(const float);
		bool operator==(const Vector3D&) const;
		float operator*(const Vector3D) const;
		Vector3D operator*(const float) const;
};

Vector3D operator*(const float, const Vector3D);

class Color
{
	public:
		unsigned char r, g, b, a; // align, dammit
		Color(unsigned char x = 0, unsigned char y = 0, unsigned char z = 0) : r(x), g(y), b(z), a(0) {} 
};

class Object
{
	public:
		Vector2D p;
		Vector2D v;
		float mass;
		float rad;
		Color color;
		float hrat;
		int id;
		
		bool dead;
		bool stopped;
		int nattached;
		int attachedTo;
		
		bool send(Socket socket);
		bool receive(Socket socket);
};

class Obstacle
{
	public:
		Vector2D p1, p2;
		Color color;
		bool deadly;
};

class Light
{
	public:
		Vector3D position;
		Color color;
};

class World
{
	public:
		World() {
			Obstacle w1, w2, w3, w4;
			w1.p1 = Vector2D(MIN_X, MIN_Y);
			w1.p2 = Vector2D(MAX_X, MIN_Y);
			w1.color = Color(255, 0, 0);
			w1.deadly = false;
			obstacles.push_back(w1);
			w2.p1 = Vector2D(MIN_X, MAX_Y);
			w2.p2 = Vector2D(MAX_X, MAX_Y);
			w2.color = Color(255, 0, 0);
			w2.deadly = false;
			obstacles.push_back(w2);
			w3.p1 = Vector2D(MIN_X, MIN_Y);
			w3.p2 = Vector2D(MIN_X, MAX_Y);
			w3.color = Color(255, 0, 0);
			w3.deadly = false;
			obstacles.push_back(w3);
			w4.p1 = Vector2D(MAX_X, MIN_Y);
			w4.p2 = Vector2D(MAX_X, MAX_Y);
			w4.color = Color(255, 0, 0);
			w4.deadly = false;
			obstacles.push_back(w4);
			
			std::ifstream mapf("map");
			int nobs = 0;
			std::string cmd;
			while (mapf >> cmd) {
				if (cmd == "obs") {
					float x1, y1, x2, y2;
					mapf >> x1 >> y1 >> x2 >> y2;
					Obstacle obs;
					obs.p1 = Vector2D(x1, y1);
					obs.p2 = Vector2D(x2, y2);
					obs.color = Color(255, 0, 0);
					obs.deadly = false;
					obstacles.push_back(obs);
				}
				else if (cmd == "sobs") {
					float x1, y1, x2, y2;
					mapf >> x1 >> y1 >> x2 >> y2;
					Obstacle obs;
					obs.p1 = Vector2D(x1, y1);
					obs.p2 = Vector2D(x2, y2);
					obs.color = Color(0, 255, 0);
					obs.deadly = true;
					obstacles.push_back(obs);
				}
				else if (cmd == "obj") {
					float x, y, mass, rad, hrat;
					mapf >> x >> y >> mass >> rad >> hrat;
					Object obj;
					obj.p = Vector2D(x, y);
					obj.v = Vector2D(0, 0);
					obj.mass = mass;
					obj.rad = rad;
					obj.hrat = hrat;
					obj.dead = false;
					obj.id = nobs++;
					obj.color = Color(255, 150, 0);
					objects[obj.id] = obj;
				}
			}
			
			Light l;
			l.position = Vector3D(0, 0, 10);
			l.color = Color(255, 255, 255);
			lights.push_back(l);
			
			Light l2;
			l2.position = Vector3D(0, 30, 20);
			l2.color = Color(255, 255, 255);
			lights.push_back(l2);
		}
		std::map<int, Object> objects;
		std::vector<Light> lights;
		std::vector<Obstacle> obstacles;
		std::vector<std::pair<char, Vector2D> > sounds;
		void doSimulation(float dt);
		
		bool sendObjects(Socket socket);
		bool receiveObjects(Socket socket);
};

#endif

