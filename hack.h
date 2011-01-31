#ifndef HACK_H
#define HACK_H

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
		Vector2D() {x= y = 0;}
		Vector2D(float a, float b) {x = a; y = b;}
		Vector2D operator+(const Vector2D);
		Vector2D operator-(const Vector2D);
		float operator*(const Vector2D);
};

inline Vector2D Vector2D::operator+(const Vector2D v)
{
  return Vector2D(x + v.x, y + v.y);
}

inline Vector2D Vector2D::operator-(const Vector2D v)
{
  return Vector2D(x - v.x, y - v.y);
}

inline float Vector2D::operator*(const Vector2D v)
{
	return x * v.x + y * v.y;
}

class Vector3D
{
	public:
		float x, y, z;
		Vector3D() {x = y = 0;}
		Vector3D(float a, float b, float c) {x = a; y = b; z = c;}
		Vector3D(Vector2D v) {x = v.x; y = v.y; z = 0;}
		Vector3D operator+(const Vector3D);
		Vector3D operator-(const Vector3D);
		float operator*(const Vector3D);
};

inline Vector3D Vector3D::operator+(const Vector3D v)
{
	return Vector3D(x + v.x, y + v.y, z + v.z);
}

inline Vector3D Vector3D::operator-(const Vector3D v)
{
	return Vector3D(x - v.x, y - v.y, z - v.z);
}

inline float Vector3D::operator*(const Vector3D v)
{
	return x * v.x + y * v.y + z * v.z;
}

class Color
{
	public:
		unsigned char r, g, b;
		Color(unsigned char x, unsigned char y, unsigned char z) {r = a; g = y; b = z;} 
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
		bool deadly;
		Color color;
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
		map<int, Object> objects;
		vector<Light> lights;
		vector<Obstacle> obstacles;
		
		bool sendObjects(Socket socket);
		bool receiveObjects(Socket socket);
};

#endif
