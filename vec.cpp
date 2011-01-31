#include "hack.h"

Vector2D Vector2D::operator+(const Vector2D& v) const
{
  Vector2D ret(x+v.x, y+v.y);
  return ret;
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
  Vector2D ret(x-v.x, y-v.y);
  return ret;
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
	return Vector2D(-y, x);
}

Vector3D Vector3D::operator+(const Vector3D& v) const
{
  Vector3D ret(x+v.x, y+v.y, z+v.z);
  return ret;
}

Vector3D Vector3D::operator-(const Vector3D& v) const
{
  Vector3D ret(x-v.x, y-v.y, z-v.z);
  return ret;
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

