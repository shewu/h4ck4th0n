#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <cstdio>

class Vector2D
{
	public:
		float x, y;

		Vector2D(float a = 0, float b = 0) : x(a), y(b) {}

		Vector2D(Vector2D const& o) : x(o.x), y(o.y) {}

		Vector2D operator+(const Vector2D& v) const {
			return Vector2D(x + v.x, y + v.y);
		}

		Vector2D operator-(const Vector2D& v) const {
			return Vector2D(x - v.x, y - v.y);
		}

		Vector2D operator-() const {
			return Vector2D(-x, -y);
		}

		Vector2D& operator+=(const Vector2D& v) {
			x += v.x;
			y += v.y;
			return *this;
		}

		Vector2D& operator-=(const Vector2D& v) {
			x -= v.x;
			y -= v.y;
			return *this;
		}

		Vector2D& operator*=(const float s) {
			x *= s;
			y *= s;
			return *this;
		}

		bool operator==(const Vector2D& v) const {
			return x == v.x && y == v.y;
		}

		float operator*(const Vector2D& v) const {
			return x * v.x + y * v.y;
		}

		Vector2D operator*(const float s) const {
			return Vector2D(s*x, s*y);
		}

		Vector2D getNormalVector() const {
			if(*this == Vector2D(0,0)) {
				fprintf(stderr, "[WARN] normalizing (0,0)!\n");
				return *this;
			}
			return Vector2D(-y, x)*(1/sqrt(x*x+y*y));
		}

		Vector2D getRotation(const float theta) const {
			return Vector2D(x*cosf(theta) - y*sinf(theta), x*sinf(theta) + y*cosf(theta));
		}

		float lengthSquared() const {
			return x*x + y*y;
		}

		float length() const {
			return sqrt(x*x + y*y);
		}

		// returns value in [0, 2pi)
		float getAngle() const {
			float theta = atan2(y, x);
			if (theta < 0.0) {
				return theta + M_PI * 2.0;
			}
			return theta;
		}

		static Vector2D getUnitVector(float theta) {
			return Vector2D(cosf(theta), sinf(theta));
		}
};

inline Vector2D operator*(const float s, const Vector2D& v) {
	return Vector2D(s * v.x, s * v.y);
}

class Vector3D
{
	public:
		float x, y, z, w;

		Vector3D(float a = 0, float b = 0, float c = 0) : x(a), y(b), z(c), w(0) {}

		Vector3D(Vector2D& v) : x(v.x), y(v.y), z(0), w(0) {}

		Vector3D operator+(const Vector3D& v) const {
			return Vector3D(x + v.x, y + v.y, z + v.z);
		}

		Vector3D operator-(const Vector3D& v) const {
			return Vector3D(x - v.x, y - v.y, z - v.z);
		}

		Vector3D operator-() const {
			return Vector3D(-x, -y, -z);
		}

		Vector3D& operator+=(const Vector3D& v) {
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Vector3D& operator-=(const Vector3D& v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		Vector3D& operator*=(const float s) {
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}

		bool operator==(const Vector3D& v) const {
			return x == v.x && y == v.y && z == v.z;
		}

		float operator*(const Vector3D& v) const {
			return x * v.x + y * v.y + z * v.z;
		}

		Vector3D operator*(const float s) const {
			return Vector3D(s*x, s*y, s*z);
		}
};

inline Vector3D operator*(const float s, const Vector3D& v) {
	return Vector3D(v.x * s, v.y * s, v.z * s);
}

#endif
