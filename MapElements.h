#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H

#include <map>
#include <vector>

#include "Vector.h"
#include "WallType.h"

class SpawnDescriptor {
	private:
		float minX, maxX, minY, maxY;

	public:
		/**
		 * Creates a SpawnDescriptor whose rectangle has vertices (x1,y1)
		 * and (x2,y2).
		 * @param x1 The first coordinate of one of the corners of the rectangle.
		 * @param y1 The second coordinate of one of the corners of the rectangle.
		 * @param x2 The first coordinate of one of corners of the rectangle.
		 * @param y2 The second coordinate of one of the corners of the rectangle.
		 */
		SpawnDescriptor(float x1, float y1, float x2, float y2) {
			if(x1 < x2) {
				minX = x1;
				maxX = x2;
			} else {
				minX = x2;
				maxX = x1;
			}
			if(y1 < y2) {
				minY = y1;
				maxY = y2;
			} else {
				minY = y2;
				maxY = y1;
			}
		}

		/**
		 * @returns the minimum X-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMinX() const {
			return minX;
		}

		/**
		 * @returns the maximum X-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMaxX() const {
			return maxX;
		}

		/**
		 * @returns the minimum Y-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMinY() const {
			return minY;
		}

		/**
		 * @returns the maximum Y-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMaxY() const {
			return maxY;
		}
};

// TODO probably kill this
class TeamDescriptor {
    private:
        unsigned teamNumber;
        unsigned minPlayers;
        unsigned maxPlayers;

        std::vector<SpawnDescriptor> spawns;

    public:
        /**
         * Empty team constructor. Does nothing.
         */
        TeamDescriptor() { }

        /**
         * Constructs a team given the team number, minimum number of players,
         * and maximum number of players.
         *
         * @param a the team number.
         * @param b the minimum number of players.
         * @param c the maximum number of players.
         */
        TeamDescriptor(unsigned a, unsigned b, unsigned c) :
            teamNumber(a), minPlayers(b), maxPlayers(c) { }

        /**
         * Gets the team number for this team.
         *
         * @return the team number.
         */
        const unsigned getTeamNumber() const {
            return teamNumber;
        }

        /**
         * Gets the minimum number of players for this team.
         *
         * @return the minimum number of players.
         */
        const unsigned getMinPlayers() const {
            return minPlayers;
        }

        /**
         * Gets the maximum number of players for this team.
         *
         * @return the maximum number of players.
         */
        const unsigned getMaxPlayers() const {
            return maxPlayers;
        }

        /**
         * Gets the spawn descriptors for this team.
         * @return the spawn descriptors.
         */
        const std::vector<SpawnDescriptor>& getSpawnDescriptors() const {
        	return spawns;
		}
};

/**
 * An immutable class that describes a Wall. Uesd by HBMap.
 */
class WallDescriptor {
	public:
		WallDescriptor(WallTypeData wallTypeData_) : _wallTypeData(wallTypeData_) { }
		WallType getWallType() const {
			return _wallTypeData.wallType;
		}

		MaterialPtr getMaterial() const {
			return _wallTypeData.material;
		}

	protected:
		WallTypeData _wallTypeData;
};

/**
 * An immutable class that describes a rectangular wall. Used by HBMap.
 */
class RectangularWallDescriptor : public WallDescriptor {
	public:
		RectangularWallDescriptor(WallTypeData wallTypeData_, Vector2D pos1_, Vector2D pos2_) : WallDescriptor(wallTypeData_), _pos1(pos1_), _pos2(pos2_) { }
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
		RoundWallDescriptor(WallTypeData wallTypeData_, Vector2D center_, float radius_, float theta1_, float theta2_) : WallDescriptor(wallTypeData_), _center(center_), _radius(radius_), _theta1(theta2_), _theta2(theta2_) { }
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
