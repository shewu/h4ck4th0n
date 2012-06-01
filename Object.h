#ifndef OBJECT_H
#define OBJECT_H

#include <cmath>

#include "Material.h"
#include "Hack.h"
#include "Packet.h"

class Object
{
	private:
		Material *material;
		unsigned id;

		static unsigned nextId;

	protected:
		mutable bool diff;
	
	private:

		// Make object non-copyable by making this private.
		Object(Object const&);
		Object& operator=(Object const&);

	public:
		/**
		 * Constructs an object with the given material.
		 * @param material The material for the object being created.
		 */
		Object(Material *material) : material(material) {
			id = (Object::nextId++);
			diff = false;
		}

		/**
		 * Constructs an Object by reading the given packet.
		 */
		Object(ReadPacket *rp);

		/**
		 * @return the material
		 */
		Material *getMaterial() const {
			return material;
		}

		/**
		 * @return the object's unique ID
		 */
		unsigned getID() const {
			return id;
		}

		bool operator<(Object const& that) const {
			return id < that.id;
		}

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket *wp) const;

		bool hasDiff() const {
			return diff;
		}

		virtual void writeDiff(WritePacket *wp) const { }
		virtual void applyDiff(ReadPacket *rp) { }

		virtual ~Object() {
			delete material;
		}
};

unsigned Object::nextId = 0;

class RectangularObject : public Object
{
	private:
		Vector2D p1, p2;

		// Make object non-copyable by making this private.
		RectangularObject(RectangularObject const&);
		RectangularObject& operator=(RectangularObject const&);

	public:
		/**
		 * Constructs a RectangularObject between the given two points
		 * and with the given material.
		 * @param a First endpoint of the wall.
		 * @param b Second endpoint of the wall.
		 * @param material The material for the object being created.
		 */
		RectangularObject(Vector2D a, Vector2D b, Material *material)
		    : Object(material), p1(a), p2(b) { }

		RectangularObject(ReadPacket *rp);

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

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket *wp) const;

		virtual ~RectangularObject();
};

class RectangularWall : public RectangularObject
{
	private:
		WallType wallType;

		// Make object non-copyable by making this private.
		RectangularWall(RectangularWall const&);
		RectangularWall& operator=(RectangularWall const&);

	public:
		/**
		 * Constructs a RectangularWall between the given two points
		 * and with the given material.
		 * @param a First endpoint of the wall.
		 * @param b Second endpoint of the wall.
		 * @param material The material for the object being created.
		 */
		RectangularWall(Vector2D a, Vector2D b, WallType wt = WT_NORMAL) :
		    RectangularObject(a, b, new Color(101, 67, 33)), wallType(wt) { }

		RectangularWall(ReadPacket *rp);

		/**
		 * @return the wall type
		 */
		WallType getWallType() const {
			return wallType;
		}

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;

		~RectangularWall();
};

class RoundObject : public Object
{
	private:

		// Make object non-copyable by making this private.
		RoundObject(RoundObject const&);
		RoundObject& operator=(RoundObject const&);

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
		RoundObject(Material *material, Vector2D& center, float radius,
		            float startAngle = 0.0, float endAngle = M_PI_2) :
		    Object(material), center(center), radius(radius), startAngle(startAngle), 
            endAngle(endAngle) { }

        RoundObject(ReadPacket *rp);

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

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;
};

class RoundWall : public RoundObject
{
	private:
		WallType wallType;

		// Make object non-copyable by making this private.
		RoundWall(RoundWall const&);
		RoundWall& operator=(RoundWall const&);

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
		RoundWall(Material *material, Vector2D center, float radius,
		            float startAngle = 0.0, float endAngle = M_PI_2,
		            WallType wt = WT_NORMAL) : 
            RoundObject(material, center, radius, startAngle, endAngle), wallType(wt) { }

		RoundWall(ReadPacket *rp);

		/**
		 * @return the wall type
		 */
		WallType getWallType() const {
			return wallType;
		}

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;
};

enum MovingObjectState {
	MOS_SPAWNING,
	MOS_ALIVE,
	MOS_SHRINKING,
	MOS_DEAD,

    MOS_NUM_STATES
};

class MovingRoundObject : public RoundObject
{
	private:

		// Make object non-copyable by making this private.
		MovingRoundObject(MovingRoundObject const&);
		MovingRoundObject& operator=(MovingRoundObject const&);

		MovingObjectState state;

		// MOS_SPAWNING
		float timeUntilSpawn;

		// MOS_SHRINKING
		MovingRoundObject *parent;
		int numChildren;

		Vector2D velocity;
		float mass;
		float heightRatio;

		bool isFlag;
		
	public:
		MovingRoundObject(Material *material, Vector2D center, float radius, float mass,
		                  float heightRatio, float timeUntilSpawn, bool isFlag) :
		    RoundObject(material, center, radius, 0.0, M_PI_2), state(MOS_SPAWNING), timeUntilSpawn(timeUntilSpawn), velocity(0.0, 0.0), mass(mass), isFlag(isFlag),
            heightRatio(heightRatio) { }

		MovingRoundObject(ReadPacket *rp);

		/**
		 * Gets the state.
		 * @return the state.
		 */
		MovingObjectState getState() const {
			return state;
		}

		/**
		 * Gets the time until spawn. Should only be called if the
		 * state is MOS_SPAWNING.
		 * @return the time until spawn
		 */
		float getTimeUntilSpawn() const {
			return timeUntilSpawn;
		}

		/**
		 * Gets the parent for shrinking, or NULL if there is no
		 * parent. Should only be called if the state is MOS_SHRINKING
		 * @return a pointer to the parent Player
		 */
		MovingRoundObject *getShrinkingParent() const {
			return parent;
		}

		/**
		 * Returns whether this object is currently shrinking.
		 * This happens if and only if the state is MOS_SHRINKING and
		 * there are no shrinking children.
		 * @return True if the object is currently shrinking.
		 */
		bool isCurrentlyShrinking() const {
			return state == MOS_SHRINKING && numChildren == 0;
		}

		/**
		 * Gets the time until spawn. Should only be called if the
		 * state is MOS_SPAWNING.
		 * @return the time until spawn
		 */
		int getNumShrinkingChildren() const {
			return numChildren;
		}

		/**
		 * @param center the new center for this object
		 */
		void setCenter(const Vector2D& center) {
			diff = true;
			this->center = center;
		}

		/**
		 * @param radius the new radius for this object
		 */
		void setRadius(float radius) {
			diff = true;
			this->radius = radius;
		}

		/**
		 * @param velocity the new velocity for this object
		 */
		void setVelocity(const Vector2D& velocity) {
			diff = true;
			this->velocity = velocity;
		}

		/**
		 * @return the velocity
		 */
		const Vector2D& getVelocity() const {
			return velocity;
		}

		/**
		 * @return the mass
		 */
		float getMass() const {
			return mass;
		}

		/**
		 * @return whether or not this object should die if it runs into the given
		 *         shrinking object.
		 */
		bool shouldDieFromShrinkingObject(MovingRoundObject const& obj) const {
			return obj.state == MOS_SHRINKING && (isFlag == obj.isFlag);
		}

		void startShrinking(MovingRoundObject *parent, Vector2D const& velocity);
		void kill();

		bool shouldDieFromWall(RectangularWall const& wall) const;

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;

		virtual void writeDiff(WritePacket *wp) const;
		virtual void applyDiff(ReadPacket *rp);
};

Material *materialsByTeamNumber[] = {new Color(0, 255, 0),
	                                 new Color(255, 0, 0),
                                     new Color(0, 0, 255)
                                    };

class FlagObject : public MovingRoundObject
{
	private:
		unsigned teamNumber;

		// Make object non-copyable by making this private.
		FlagObject(FlagObject const&);
		FlagObject& operator=(FlagObject const&);

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
		FlagObject(unsigned teamNumber, float timeUntilSpawn) :
		    MovingRoundObject(materialsByTeamNumber[teamNumber],
		                      Vector2D(), FlagObject::FLAG_RADIUS,
                              FlagObject::FLAG_MASS, FlagObject::FLAG_HEIGHT_RATIO, timeUntilSpawn, true),
            teamNumber(teamNumber) { }
		
		FlagObject(ReadPacket *rp);

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
		static FlagObject *readFromPacket(ReadPacket *rp);

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;
};

float FlagObject::FLAG_RADIUS = 1.0f;
float FlagObject::FLAG_MASS = 1.2f;
float FlagObject::FLAG_HEIGHT_RATIO = 2.0f;

class PlayerObject : public MovingRoundObject
{
	private:
		unsigned teamNumber;

        static float PLAYER_RADIUS;
        static float PLAYER_MASS;
        static float PLAYER_HEIGHT_RATIO;

		// Make object non-copyable by making this private.
		PlayerObject(PlayerObject const&);
		PlayerObject& operator=(PlayerObject const&);

	public:
		/**
		 * Creates a Player with the given material and team.
		 * @param material The material for this player.
		 * @param teamNumber the number of this player's team.
		 */
		PlayerObject(unsigned teamNumber, float timeUntilSpawn) :
		    MovingRoundObject(materialsByTeamNumber[teamNumber],
		                      Vector2D(),
		                      PlayerObject::PLAYER_RADIUS, PlayerObject::PLAYER_MASS
		                      PlayerObject::PLAYER_HEIGHT_RATIO, timeUntilSpawn, false),
                              teamNumber(teamNumber) { }

		/**
		 * Gets the team number.
		 * @return the team number.
		 */
		unsigned getTeamNumber() const {
			return teamNumber;
		}

		/**
		 * Creates a Player object by reading a ReadPacket
		 * @param rp ReadPacket to read from
		 */
		PlayerObject(ReadPacket *rp);

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;
};

float PlayerObject::PLAYER_RADIUS = 1;
float PlayerObject::PLAYER_MASS = 1;
float PlayerObject::PLAYER_HEIGHT_RATIO = 1;

#endif

