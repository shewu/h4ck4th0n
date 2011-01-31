#ifndef HACK_H
#define HACK_H

#include <map>
#include <vector>

#define MIN_X -10.0f
#define MAX_X 10.0f
#define MIN_Y -10.0f
#define MAX_Y 10.0f

#define KEYPRESS_ACCELERATION 2.0f

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
			
			Object o;
			o.p = Vector2D(0, 0);
			o.v = Vector2D(0, 0);
			o.mass = 1.5;
			o.rad = 1;
			o.color = Color(255, 150, 0);
			o.h = 1.5;
			o.id = 0;
			objects[0] = o;
		}
		std::map<int, Object> objects;
		std::vector<Light> lights;
		std::vector<Obstacle> obstacles;
		void doSimulation(float dt);
		
		bool sendObjects(Socket socket);
		bool receiveObjects(Socket socket);
};

#endif

