#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H

#include <map>
#include <vector>

#include "Vector.h"
#include "WallType.h"

class SpawnComponent {
	public:
		virtual float getArea() const = 0;
		virtual Vector2D getRandomPoint() const = 0;
};

class SpawnDescriptor {
	public:
		SpawnDescriptor() { }

		void addComponent(SpawnComponent* sc) {
			_components.push_back(sc);
		}

		Vector2D getRandomPoint() const;

	private:
		std::vector<SpawnComponent*> _components;
};

class SpawnComponentPoint : public SpawnComponent {
	public:
		SpawnComponentPoint(Vector2D const& point) : _point(point) { }
		float getArea() const;
		Vector2D getRandomPoint() const;

	private:
		Vector2D _point;
};

class SpawnComponentLine : public SpawnComponent {
	public:
		SpawnComponentLine(Vector2D const& point1, Vector2D const& point2) : _point1(point1), _point2(point2) { }
		float getArea() const;
		Vector2D getRandomPoint() const;

	private:
		Vector2D _point1;
		Vector2D _point2;
};

class SpawnComponentRectangle : public SpawnComponent {
	public:
		SpawnComponentRectangle(Vector2D const& point1, Vector2D const& point2) : _point1(point1), _point2(point2) { }
		float getArea() const;
		Vector2D getRandomPoint() const;

	private:
		Vector2D _point1;
		Vector2D _point2;
};

class SpawnComponentTriangle : public SpawnComponent {
	public:
		SpawnComponentTriangle(Vector2D const& point1, Vector2D const& point2, Vector2D const& point3) : _point1(point1), _point2(point2), _point3(point3) { }
		float getArea() const;
		Vector2D getRandomPoint() const;

	private:
		Vector2D _point1;
		Vector2D _point2;
		Vector2D _point3;
};

/**
 * An immutable class that describes a Wall. Uesd by HBMap.
 */
class WallDescriptor {
	public:
		WallDescriptor(WallTypeData wallTypeData_, float bouncinessCoefficient_) : 
			_wallTypeData(wallTypeData_),
			_bouncinessCoefficient(bouncinessCoefficient_) { }

		WallType getWallType() const {
			return _wallTypeData.wallType;
		}

		MaterialPtr getMaterial() const {
			return _wallTypeData.material;
		}

		float getBouncinessCoefficient() const {
			return _bouncinessCoefficient;
		}

		constexpr static float kDefaultBouncinessCoefficient = 1.0;

	protected:
		WallTypeData _wallTypeData;
		float _bouncinessCoefficient;
};

/**
 * An immutable class that describes a rectangular wall. Used by HBMap.
 */
class RectangularWallDescriptor : public WallDescriptor {
	public:
		RectangularWallDescriptor(WallTypeData wallTypeData_, Vector2D pos1_, Vector2D pos2_, float bouncinessCoefficient_) : WallDescriptor(wallTypeData_, bouncinessCoefficient_), _pos1(pos1_), _pos2(pos2_) { }
		Vector2D getPos1() const {
			return this->_pos1;
		}
		Vector2D getPos2() const {
			return this->_pos2;
		}

	private:
		Vector2D _pos1;
		Vector2D _pos2;
};

/**
 * An immutable class that describes a round wall. Used by HBMap.
 */
class RoundWallDescriptor : public WallDescriptor {
	public:
		RoundWallDescriptor(WallTypeData wallTypeData_, Vector2D center_, float radius_, float theta1_, float theta2_, float bouncinessCoefficient_) : WallDescriptor(wallTypeData_, bouncinessCoefficient_), _center(center_), _radius(radius_), _theta1(theta1_), _theta2(theta2_) { }
		Vector2D getCenter() const {
			return this->_center;
		}
		float getRadius() const {
			return this->_radius;
		}
		float getTheta1() const {
			return this->_theta1;
		}
		float getTheta2() const {
			return this->_theta2;
		}

	private:
		Vector2D _center;
		float _radius;
		float _theta1;
		float _theta2;
};

class Floor {
	public:
		Floor(float minX, float minY, float maxX, float maxY, float frictionCoeff) :
			_minX(minX), _minY(minY), _maxX(maxX), _maxY(maxY),
			_frictionCoeff(frictionCoeff) { }

		/**
		 * Returns true if the point resides in the region covered by this floor, and
		 * false otherwise. In the case that it returns true, it puts the friction coefficient
		 * of the floor at that point in the 'coeff' argument. In the false case, coeff can be
		 * anything.
		 */
		bool getFrictionCoeff(Vector2D p, float& coeff) const {
			if (p.x >= _minX && p.x <= _maxX && p.y >= _minY && p.y <= _maxY) {
				coeff = _frictionCoeff;
				return true;
			}
			return false;
		}

	private:
		float _minX, _minY, _maxX, _maxY;
		float _frictionCoeff;
};



#endif
