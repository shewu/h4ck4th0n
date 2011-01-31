class Vector2D
{
  public:
		float x, y;
};

class Vector3D
{
	public:
		float x, y, z;
};

class Color
{
	public:
		unsigned char r, g, b;
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
