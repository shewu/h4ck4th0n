#ifndef OBJECT_H
#define OBJECT_H

#include <cmath>

#include "Material.h"
#include "hack.h"
#include "packet.h"

/*
class Object
{
	public:
		Vector2D p;
		Vector2D v;
		float mass;
		float rad;
		Color color;
		float hrat;
		int id;
		
		bool dead;
		bool stopped;
		int spawnl;
		int spawny;
		double time_until_spawn;
		int nattached;
		int attachedTo;
		int player;
		int flag;
		
		void write_data(WritePacket* wp);
		void read_data(ReadPacket* rp);
};
*/

class Object
{
	private:
		Material material;
		unsigned id;

		static unsigned nextId;

	public:
		/**
		 * Constructs an object with the given material.
		 * @param material The material for the object being created.
		 */
		Object(Material material) : material(material) {
			id = (Object::nextId++);
		}

		/**
		 * @return the material
		 */
		const Material& getMaterial() const {
			return material;
		}

		/**
		 * @return the object's unique ID
		 */
		unsigned getID() const {
			return id;
		}

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket *wp);
};

unsigned Object::nextId = 0;

class RectangularObject : public Object
{
	private:
		Vector2D p1, p2;

	public:
		/**
		 * Constructs a RectangularObject between the given two points
		 * and with the given material.
		 * @param a First endpoint of the wall.
		 * @param b Second endpoint of the wall.
		 * @param material The material for the object being created.
		 */
		RectangularObject(Vector2D a, Vector2D b, Material material)
		    : Object(material), p1(a), p2(b) { }

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

const std::string wallTypeStrings[] = {
    "normal", "deadly", "bouncy"
};

class RectangularWall : public RectangularObject
{
	private:
		WallType wallType;

	public:
		/**
		 * Constructs a RectangularWall between the given two points
		 * and with the given material.
		 * @param a First endpoint of the wall.
		 * @param b Second endpoint of the wall.
		 * @param material The material for the object being created.
		 */
		RectangularWall(Vector2D a, Vector2D b, Material material, WallType wt = WT_NORMAL) :
		    RectangularObject(a, b, material), wallType(wt) { }

		/**
		 * @return the wall type
		 */
		WallType getWallType() {
			return wallType;
		}

		/**
		 * Creates a RectangularWall object by reading a ReadPacket
		 * @param rp ReadPacket to read from
		 * @return a RectangularWall object read from the ReadPacket
		 */
		static RectangularWall readFromPacket(ReadPacket *rp);
};

class RoundObject : public Object
{
	protected:
		Vector2D center;
		float radius;
		float startAngle, endAngle;

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
		RoundObject(Material material, Vector2D& center, float radius,
		            float startAngle = 0.0, float endAngle = M_PI_2) :
		    Object(material), center(center), radius(radius), startAngle(startAngle), 
            endAngle(endAngle) { }

		/**
		 * @return the center
		 */
		const Vector2D& getCenter() const {
			return center;
		}

		/**
		 * @return the radius
		 */
		float getRadius() const {
			return radius;
		}

		/**
		 * @return the starting angle
		 */
		float getStartAngle() const {
			return startAngle;
		}

		/**
		 * @return the ending angle
		 */
		float getEndAngle() const {
			return endAngle;
		}
};

class RoundWall : public RoundObject
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
		RoundWall(Material material, Vector2D center, float radius,
		            float startAngle = 0.0, float endAngle = M_PI_2,
		            WallType wt = WT_NORMAL) : 
            RoundObject(material, center, radius, startAngle, endAngle), wallType(wt) { }

		/**
		 * @return the wall type
		 */
		WallType getWallType() const {
			return wallType;
		}

		/**
		 * Creates a RoundWall object by reading a ReadPacket
		 * @param rp ReadPacket to read from
		 * @return a RoundWall object read from the ReadPacket
		 */
		static RoundWall readFromPacket(ReadPacket *rp);
};

class MovingRoundObject : public RoundObject
{
	private:
		Vector2D velocity;
		float mass;
		float heightRatio;
		
	public:
		MovingRoundObject(Material material, Vector2D center, float radius, float mass,
		                  float heightRatio) :
		    RoundObject(material, center, radius, 0.0, M_PI_2), velocity(0.0, 0.0), mass(mass), 
            heightRatio(heightRatio) { }

		/**
		 * @param center the new center for this object
		 */
		void setCenter(const Vector2D& center) {
			this->center = center;
		}

		/**
		 * @param velocity the new velocity for this object
		 */
		void setVelocity(const Vector2D& velocity) {
			this->velocity = velocity;
		}

		/**
		 * @return the velocity
		 */
		const Vector2D& getWallType() const {
			return velocity;
		}

		/**
		 * @return the mass
		 */
		float getMass() const {
			return mass;
		}
};

class Flag : public MovingRoundObject
{
	private:
		unsigned teamNumber;

	public:
        static float FLAG_RADIUS;
        static float FLAG_MASS;
        static float FLAG_HEIGHT_RATIO;

		/**
		 * Constructs a flag with the given material, center, and team.
		 * @param material Material to use for the flag.
		 * @param center The center of the flag.
		 * @param teamNumber The team number for this flag.
		 */
		Flag(Material material, Vector2D center, unsigned teamNumber) :
		    MovingRoundObject(material, center, Flag::FLAG_RADIUS,
                Flag::FLAG_MASS, Flag::FLAG_HEIGHT_RATIO),
            teamNumber(teamNumber) { }
		
		/**
		 * @return the team number
		 */
		unsigned getTeamNumber() const {
			return teamNumber;
		}

		/**
		 * Creates a Flag object by reading a ReadPacket
		 * @param rp ReadPacket to read from
		 * @return a Flag object read from the ReadPacket
		 */
		static Flag readFromPacket(ReadPacket *rp);
};

float Flag::FLAG_RADIUS = 1.0f;
float Flag::FLAG_MASS = 1.2f;
float Flag::FLAG_HEIGHT_RATIO = 2.0f;

enum PlayerState {
	PS_SPAWNING,
	PS_ALIVE,
	PS_SHRINKING,
	PS_DEAD,

    PS_NUM_STATES
};

class Player : public MovingRoundObject
{
	private:
		PlayerState playerState;
		unsigned teamNumber;

		// PS_SPAWNING
		float timeUntilSpawn;

		// PS_SHRINKING
		Player *parent;
		int numChildren;

        static float PLAYER_RADIUS;
        static float PLAYER_MASS;
        static float PLAYER_HEIGHT_RATIO;

	public:
		/**
		 * Creates a Player with the given material and team.
		 * @param material The material for this player.
		 * @param teamNumber the number of this player's team.
		 */
		Player(Material material, unsigned teamNumber) :
		    MovingRoundObject(material, Vector2D(), Player::PLAYER_RADIUS, Player::PLAYER_MASS, Player::PLAYER_HEIGHT_RATIO),
            playerState(PS_DEAD), teamNumber(teamNumber) { }

		/**
		 * Gets the player state.
		 * @return the player state.
		 */
		PlayerState getPlayerState() const {
			return playerState;
		}

		/**
		 * Gets the team number.
		 * @return the team number.
		 */
		unsigned getTeamNumber() const {
			return teamNumber;
		}

		/**
		 * Gets the time until spawn. Should only be called if the
		 * playerState is PS_SPAWNING.
		 * @return the time until spawn
		 */
		float getTimeUntilSpawn() const {
			return timeUntilSpawn;
		}

		/**
		 * Gets the parent for shrinking, or NULL if there is no
		 * parent. Should only be called if the playerState is PS_SHRINKING
		 * @return a pointer to the parent Player
		 */
		Player *getShrinkingParent() const {
			return parent;
		}

		/**
		 * Gets the time until spawn. Should only be called if the
		 * playerState is PS_SPAWNING.
		 * @return the time until spawn
		 */
		int getNumShrinkingChildren() const {
			return numChildren;
		}

		/**
		 * Creates a Player object by reading a ReadPacket
		 * @param rp ReadPacket to read from
		 * @return a Player object read from the ReadPacket
		 */
		static Player readFromPacket(ReadPacket *rp);
};

float Player::PLAYER_RADIUS = 1;
float Player::PLAYER_MASS = 1;
float Player::PLAYER_HEIGHT_RATIO = 1;

#endif

