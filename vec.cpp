#include <cstdio>
#include <cmath>

#include "Vector.h"

Vector2D Vector2D::operator+(const Vector2D& v) const
{
  return Vector2D(x+v.x, y+v.y);
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
  return Vector2D(x-v.x, y-v.y);
}

Vector2D Vector2D::operator-() const
{
	return Vector2D(-x, -y);
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

Vector2D& Vector2D::operator*=(const float s)
{
	x *= s;
	y *= s;
	return *this;
}

bool Vector2D::operator==(const Vector2D& v) const
{
  return x == v.x && y == v.y;
}

float Vector2D::operator*(const Vector2D& v) const
{
  return x * v.x + y * v.y;
}

Vector2D Vector2D::operator*(const float s) const
{
	return Vector2D(s*x, s*y);
}

Vector2D Vector2D::getNormalVector() const
{
	if(*this == Vector2D(0,0))
	{
		fprintf(stderr, "[WARN] normalizing (0,0)!\n");
		return *this;
	}
	return Vector2D(-y, x)*(1/sqrt(x*x+y*y));
}

Vector2D operator*(const float s, const Vector2D v) {
	return Vector2D(v.x*s,v.y*s);
}

Vector3D Vector3D::operator+(const Vector3D& v) const
{
  return Vector3D(x+v.x, y+v.y, z+v.z);
}

Vector3D Vector3D::operator-(const Vector3D& v) const
{
  return Vector3D(x-v.x, y-v.y, z-v.z);
}

Vector3D Vector3D::operator-() const
{
	return Vector3D(-x, -y, -z);
}

Vector3D& Vector3D::operator+=(const Vector3D& v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}

Vector3D& Vector3D::operator*=(const float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

bool Vector3D::operator==(const Vector3D& v) const
{
  return x == v.x && y == v.y && z == v.z;
}

float Vector3D::operator*(const Vector3D v) const
{
  return x * v.x + y * v.y + z * v.z;
}

Vector3D Vector3D:: operator*(const float s) const
{
	return Vector3D(s*x, s*y, s*z);
}

Vector3D operator*(const float s, const Vector3D v) {
	return Vector3D(v.x*s,v.y*s,v.z*s);
}

