#ifndef OBJECT_H
#define OBJECT_H

#include <cmath>

#include "Material.h"
#include "hack.h"

class Object
{
	private:
		Material material;

	public:
		/**
		 * Constructs an object with the given material.
		 * @param m The material for the object being created.
		 */
		Object(Material m) : material(m) { }

		/**
		 * @return the material
		 */
		const Material& getMaterial() const {
			return material;
		}
};

class RectangularObject : Object
{
	private:
		Vector2D p1, p2;

	public:
		/**
		 * Constructs a RectangularObject between the given two points
		 * and with the given material.
		 * @param a First endpoint of the wall.
		 * @param b Second endpoint of the wall.
		 * @param m The material for the object being created.
		 */
		RectangularObject(Vector2D a, Vector2D b, Material m)
		    : p1(a), p2(b), Object(m) { }

		/**
		 * @return the first endpoint of the object
		 */
		const Vector2D& getP1() const {
			return p1;
		}

		/**
		 * @return the second endpoint of the object
		 */
		const Vector2D& getP2() const {
			return p2;
		}
};

enum WallType
{
    WT_INVALID = -1,
    WT_NORMAL,
    WT_DEADLY,
    WT_BOUNCY,

    NUM_WALLTYPES // always the last mode
};

class RectangularWall : RectangularObject
{
	private:
		WallType wallType;

	public:
		/**
		 * Constructs a RectangularWall between the given two points
		 * and with the given material.
		 * @param a First endpoint of the wall.
		 * @param b Second endpoint of the wall.
		 * @param m The material for the object being created.
		 */
		RectangularWall(Vector2D a, Vector2D b, Material m, WallType wt = WT_NORMAL) :
		    wallType(wt) : RectangularObject(a, b, m) { }

		/**
		 * @return the wall type
		 */
		const WallType& getWallType() const {
			return wallType;
		}
};

class RoundObject : Object
{
	private:
		float radius;
		float startAngle, endAngle;
	
	protected:
		float center;

	public:
		/**
		 * Constructs a RoundObject with the given material.
		 * The object is represented by an arc which goes from startAngle
		 * to endAngle. If endAngle > startAngle, it goes counter-clockwise from
		 * startAngle to endAngle; otherwise, it goes clockwise.
		 * @param material The material for the object being created.
		 * @param center The center of the circle for the moving object.
		 * @param radius The radius of the circle for the moving object.
		 * @param startAngle The starting angle for the arc.
		 * @param endAngle The ending angle for the arc.
		 */
		RoundObject(Material material, Vector2D center, float radius,
		            float startAngle = 0.0, float endAngle = M_PI_2) :
		    center(center), radius(radius), startAngle(startAngle), endAngle(endAngle),
		    Object(material) { }

		/**
		 * @return the center
		 */
		const Vector2D& getCenter() const {
			return center;
		}

		/**
		 * @return the radius
		 */
		const float& getRadius() const {
			return radius;
		}

		/**
		 * @return the starting angle
		 */
		const float& getStartAngle() const {
			return startAngle;
		}

		/**
		 * @return the ending angle
		 */
		const float& getEndAngle() const {
			return endAngle;
		}
}

class RoundWall : RoundObject
{
	private:
		WallType wallType;

	public:
		/**
		 * Constructs a RoundWall with the given material.
		 * The object is represented by an arc which goes from startAngle
		 * to endAngle. If endAngle > startAngle, it goes counter-clockwise from
		 * startAngle to endAngle; otherwise, it goes clockwise.
		 * @param material The material for the object being created.
		 * @param center The center of the circle for the moving object.
		 * @param radius The radius of the circle for the moving object.
		 * @param startAngle The starting angle for the arc.
		 * @param endAngle The ending angle for the arc.
		 * @param wt The wall type for this wall.
		 */
		RoundObject(Material material, Vector2D center, float radius,
		            float startAngle = 0.0, float endAngle = M_PI_2,
		            WallType wt = WT_NORMAL) :
		     wallType(wt), RoundObject(material, center, radius, 
		                               startAngle, endAngle)

		/**
		 * @return the wall type
		 */
		const WallType& getWallType() const {
			return wallType;
		}
}

#endif
