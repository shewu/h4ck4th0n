class Vector2D
{
  public:
		float x, y;
		Vector2D() : x(0), y(0) {}
		Vector2D(float a, float b) : x(a), y(b) {}
		Vector2D& operator+(const Vector2D&) const;
		Vector2D& operator-(const Vector2D&) const;
		Vector2D& operator+=(const Vector2D&);
		Vector2D& operator-=(const Vector2D&);
		float operator*(const Vector2D&) const;
};

Vector2D& Vector2D::operator+(const Vector2D& v) const
{
  return Vector2D(x + v.x, y + v.y);
}

Vector2D& Vector2D::operator-(const Vector2D& v) const
{
  return Vector2D(x - v.x, y - v.y);
}

Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

float Vector2D::operator*(const Vector2D& v) const
{
	return x * v.x + y * v.y;
}

class Vector3D
{
	public:
		float x, y, z, w;
		Vector3D() : x(0), y(0), z(0), w(0) {}
		Vector3D(float a, float b, float c) : x(a), y(b), z(c), w(0) {}
		Vector3D(Vector2D v) : x(v.x), y(v.y), z(0), w(0) {}
		Vector3D operator+(const Vector3D) const;
		Vector3D operator-(const Vector3D) const;
		float operator*(const Vector3D) const;
};

Vector3D Vector3D::operator+(const Vector3D v) const
{
	return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator-(const Vector3D v) const
{
	return Vector3D(x - v.x, y - v.y, z - v.z);
}

float Vector3D::operator*(const Vector3D v) const
{
	return x * v.x + y * v.y + z * v.z;
}

class Color
{
	public:
		unsigned char r, g, b, a; // align, dammit
		Color(unsigned char x, unsigned char y, unsigned char z) : r(x), g(y), b(z), a(0) {} 
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
};

