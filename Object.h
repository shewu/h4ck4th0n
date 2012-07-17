#ifndef OBJECT_H
#define OBJECT_H

#include <assert.h>

#include <cmath>
#include <functional>
#include <memory>

#include "Material.h"
#include "Hack.h"
#include "Packet.h"

class Object
{
	private:
		MaterialPtr material;
		int id;
		unsigned refCount;

		static unsigned nextId;

		// Make object non-copyable by making this private.
		Object(Object const&);
		Object& operator=(Object const&);

	public:
		static const MaterialPtr materialsByTeamNumber[];

		/**
		 * Constructs an object with the given material.
		 * @param material The material for the object being created.
		 */
		Object(MaterialPtr material) : material(material), refCount(0) {
			id = (Object::nextId++);
		}

		/**
		 * Constructs an Object by reading the given packet.
		 */
		explicit Object(ReadPacket *rp);

		/**
		 * @return the material
		 */
		MaterialPtr getMaterial() const {
			return material;
		}

		/**
		 * Sets the material.
		 * @param m should not be destructed until after PhysicsWorld is
		 */
		void setMaterial(MaterialPtr m) {
			material = m;
		}

		/**
		 * @return the object's unique ID
		 */
		int getID() const {
			return id;
		}

		bool operator<(Object const& that) const {
			return id < that.id;
		}

		int getRefCount() const {
			return refCount;
		}

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket *wp) const;

		virtual ~Object() { }

	template<class> friend class ObjectPtr;
};

template <class T>
class ObjectPtr {
	public: 
		explicit ObjectPtr(T* obj = NULL) : ptr_(obj) {
			if (obj != NULL) {
				obj->refCount++;
			}
		}

		~ObjectPtr() {
			if (ptr_ != NULL) {
				ptr_->refCount--;
			}
		}

		template <class U>
		ObjectPtr(ObjectPtr<U> const& other) {
			ptr_ = (T *) other.ptr_;
			if (ptr_ != NULL) {
				ptr_->refCount++;
			}
		}

		template <class U>
		ObjectPtr<T>& operator=(ObjectPtr<U> const& other) {
			if (ptr_ != NULL) {
				ptr_->refCount--;
			}
			ptr_ = (T *) other.ptr_;
			if (ptr_ != NULL) {
				ptr_->refCount++;
			}
		}

		T& operator*() {
			return *ptr_;
		}

		T* operator->() {
			return ptr_;
		}

		bool empty() const {
			return ptr_ == NULL;
		}

	private:
		T* ptr_;

	template<class U> friend class ObjectPtr;
};

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
		RectangularObject(Vector2D a, Vector2D b, MaterialPtr material)
			: Object(material), p1(a), p2(b) { }

		explicit RectangularObject(ReadPacket *rp);

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

		friend class PhysicsWorld;
		friend class UnholyGameViewController;
		friend class HolyGameViewController;
		friend class GameViewController;
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
			RectangularObject(a, b, MaterialPtr(new Color(101, 67, 33))), wallType(wt) { }

		explicit RectangularWall(ReadPacket *rp);

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
		RoundObject(MaterialPtr material, Vector2D const& center, float radius,
				float startAngle = 0.0, float endAngle = M_PI_2) :
			Object(material), center(center), radius(radius), startAngle(startAngle), 
			endAngle(endAngle) { }

		explicit RoundObject(ReadPacket *rp);

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

		friend class PhysicsWorld;
		friend class UnholyGameViewController;
		friend class HolyGameViewController;
		friend class GameViewController;

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
		RoundWall(MaterialPtr material, Vector2D center, float radius,
				float startAngle = 0.0, float endAngle = M_PI_2,
				WallType wt = WT_NORMAL) : 
			RoundObject(material, center, radius, startAngle, endAngle), wallType(wt) { }

		explicit RoundWall(ReadPacket *rp);

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

		float mass;
		float heightRatio;

		// MOS_SPAWNING
		float timeUntilSpawn;

		// MOS_SHRINKING
		MovingRoundObject *parent;
		int numChildren;

		int regionNumber;

		// Callbacks
		std::function<void()> onSpawnCallback;
		std::function<void()> onDeathCallback;

		Vector2D velocity;

		// helpers used by PhysicsWorld
		void startShrinking(MovingRoundObject *parent, Vector2D const& velocity);
		void kill();

	public:
		MovingRoundObject(MaterialPtr material, float radius, float mass, 
				float heightRatio, float timeUntilSpawn, int regionNumber,
				std::function<void()> const& onSpawnCallback,
				std::function<void()> const& onDeathCallback) :
			RoundObject(material, Vector2D(0.0,0.0), radius, 0.0f, (float)M_PI_2),
			state(MOS_SPAWNING),
			mass(mass),
			heightRatio(heightRatio),
			timeUntilSpawn(timeUntilSpawn),
			regionNumber(regionNumber),
			onSpawnCallback(onSpawnCallback),
			onDeathCallback(onDeathCallback),
			velocity(0.0, 0.0) { }

		static const float kPlayerRadius;
		static const float kPlayerMass;
		static const float kPlayerHeightRatio;

		static const float kFlagRadius;
		static const float kFlagMass;
		static const float kFlagHeightRatio;

		explicit MovingRoundObject(ReadPacket *rp);

		/**
		 * Gets the region number.
		 * @return the region number.
		 */
		int getRegionNumber() const {
			return regionNumber;
		}

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
		 * @param velocity the new velocity for this object
		 */
		void setVelocity(const Vector2D& velocity) {
			assert(state == MOS_ALIVE);
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

		void instantKill() {
			state = MOS_DEAD;
		}

		void setSpawnCallback(std::function<void()> callback) {
			onSpawnCallback = callback;
		}
		void setDeathCallback(std::function<void()> callback) {
			onDeathCallback = callback;
		}

		/**
		 * Writes the object to a WritePacket
		 * @param wp the WritePacket to write this object to
		 */
		virtual void writeToPacket(WritePacket* wp) const;

		friend class PhysicsWorld;
		friend class UnholyGameViewController;
		friend class HolyGameViewController;
		friend class GameViewController;
};

#endif
