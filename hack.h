#ifndef HACK_H
#define HACK_H

#include <map>
#include <vector>

class Socket
{
	public:
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
		Vector2D() : x(0), y(0) {}
		Vector2D(float a, float b) : x(a), y(b) {}
		Vector2D operator+(const Vector2D&) const;
		Vector2D operator-(const Vector2D&) const;
		Vector2D& operator+=(const Vector2D&);
		Vector2D& operator-=(const Vector2D&);
		bool operator==(const Vector2D&) const;
		float operator*(const Vector2D&) const;
};

class Vector3D
{
	public:
		float x, y, z, w;
		Vector3D() : x(0), y(0), z(0), w(0) {}
		Vector3D(float a, float b, float c) : x(a), y(b), z(c), w(0) {}
		Vector3D(Vector2D& v) : x(v.x), y(v.y), z(0), w(0) {}
		Vector3D operator+(const Vector3D&) const;
		Vector3D operator-(const Vector3D&) const;
		Vector3D& operator+=(const Vector3D&);
		Vector3D& operator-=(const Vector3D&);
		bool operator==(const Vector3D&) const;
		float operator*(const Vector3D) const;
};

class Color
{
	public:
		unsigned char r, g, b, a; // align, dammit
		Color(unsigned char x, unsigned char y, unsigned char z) : r(x), g(y), b(z), a(0) {} 
		Color() : r(0), g(0), b(0), a(0) {}
};

class Object
{
	public:
		Vector2D p;
		Vector2D v;
		float mass;
		float rad;
		Color color;
		float h;
		int id;
		
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
		std::map<int, Object> objects;
		std::vector<Light> lights;
		std::vector<Obstacle> obstacles;
		void doSimulation(float dt);
		
		bool sendObjects(Socket socket);
		bool receiveObjects(Socket socket);
};

#endif

