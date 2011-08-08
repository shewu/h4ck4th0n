#ifndef HACK_H
#define HACK_H

#include <map>
#include <vector>
#include "constants.h"
#include "socket.h"

#ifdef DEBUG
#define P(x) printf x
#else
#define P(x)
#endif

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
		int spawnl;
		int spawny;
		double spawnTime;
		int nattached;
		int attachedTo;
		int player;
		int flag;
		
		void send(SocketConnection* sc);
		char* get(char* buf);
};

class Spawn {
	public:
		float xmin, xmax, ymin, ymax;
		float mass;
		float rad;
		Color color;
		float hrat;
};

class Obstacle
{
	public:
		Vector2D p1, p2;
		Color color;
		bool sticky;
		int flag;
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
		World();
		int spawn(int spawnl, int player, int flag);
		
		float minX, maxX, minY, maxY;
		std::map<int, std::vector<Spawn> > spawns;
		std::map<int, Object> objects;
		std::vector<Light> lights;
		std::vector<Obstacle> obstacles;
		std::vector<std::pair<char, Vector2D> > sounds;
		void doSimulation(float dt);
		
		void sendObjects(SocketConnection* sc, int obj);
		int receiveObjects(SocketConnection* sc, int& obj);
};

#endif
