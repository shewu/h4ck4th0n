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

Vector2D Vector2D::operator+(const Vector2D v)
{
  return Vector2D(x + v.x, y + v.y);
}

Vector2D Vector2D::operator-(const Vector2D v)
{
  return Vector2D(x - v.x, y - v.y);
}

float Vector2D::operator*(const Vector2D v)
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

Vector3D Vector3D::operator+(const Vector3D v)
{
	return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator-(const Vector3D v)
{
	return Vector3D(x - v.x, y - v.y, z - v.z);
}

float Vector3D::operator*(const Vector3D v)
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
};
