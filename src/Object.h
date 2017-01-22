#ifndef OBJECT_H
#define OBJECT_H

#include <assert.h>

#include <cmath>
#include <functional>
#include <memory>

#include "Material.h"
#include "Packet.h"
#include "Vector.h"
#include "WallType.h"

class Object {
private:
    MaterialPtr material;
    int id;
    unsigned refCount;

    static unsigned nextId;

    // Make object non-copyable by making this private.
    Object(Object const &);
    Object &operator=(Object const &);

public:
    static const MaterialPtr materialsByTeamNumber[];

    /**
     * Constructs an object with the given material.
     * @param material The material for the object being created.
     */
    explicit Object(MaterialPtr material) : material(material), refCount(0) {
        id = (Object::nextId++);
    }

    /**
     * Constructs an Object by reading the given packet.
     */
    explicit Object(ReadPacket *rp);

    /**
     * @return the material
     */
    MaterialPtr getMaterial() const { return material; }

    /**
     * Sets the material.
     * @param m should not be destructed until after PhysicsWorld is
     */
    void setMaterial(MaterialPtr m) { material = m; }

    /**
     * @return the object's unique ID
     */
    int getID() const { return id; }

    bool operator<(Object const &that) const { return id < that.id; }

    int getRefCount() const { return refCount; }

    /**
     * Writes the object to a WritePacket
     * @param wp the WritePacket to write this object to
     */
    virtual void writeToPacket(WritePacket *wp) const;

    virtual ~Object() {}

    template <class>
    friend class ObjectPtr;
};

template <class T>
class ObjectPtr {
public:
    explicit ObjectPtr(T *obj = NULL) : ptr_(obj) {
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
    ObjectPtr(ObjectPtr<U> const &other) {
        ptr_ = (T *)other.ptr_;
        if (ptr_ != NULL) {
            ptr_->refCount++;
        }
    }

    template <class U>
    ObjectPtr<T> &operator=(ObjectPtr<U> const &other) {
        if (ptr_ != NULL) {
            ptr_->refCount--;
        }
        ptr_ = (T *)other.ptr_;
        if (ptr_ != NULL) {
            ptr_->refCount++;
        }
    }

    T &operator*() { return *ptr_; }

    T *operator->() { return ptr_; }

    bool empty() const { return ptr_ == NULL; }

private:
    T *ptr_;

    template <class U>
    friend class ObjectPtr;
};

class Wall : public Object {
protected:
    WallType wallType;
    float bouncinessCoefficient;

public:
    Wall(MaterialPtr material, WallType wallType, float bouncinessCoefficient)
        : Object(material),
          wallType(wallType),
          bouncinessCoefficient(bouncinessCoefficient) {}

    WallType getWallType() const { return wallType; }

    Wall(ReadPacket *rp);
    virtual void writeToPacket(WritePacket *wp) const;

    friend class PhysicsWorld;
    friend class UnholyGameViewController;
    friend class HolyGameViewController;
    friend class MultiGameViewController;
    friend class GameViewController;
};

class RectangularWall : public Wall {
protected:
    Vector2D p1, p2;

public:
    RectangularWall(MaterialPtr material, WallType wallType,
                    float bouncinessCoefficient, Vector2D p1, Vector2D p2)
        : Wall(material, wallType, bouncinessCoefficient), p1(p1), p2(p2) {}

    RectangularWall(ReadPacket *rp);
    virtual void writeToPacket(WritePacket *wp) const;

    friend class PhysicsWorld;
    friend class UnholyGameViewController;
    friend class HolyGameViewController;
    friend class MultiGameViewController;
    friend class GameViewController;
};

class RoundWall : public Wall {
private:
    Vector2D center;
    float radius, theta1, theta2;

public:
    // TODO these should just accept Descriptors as arguments
    RoundWall(MaterialPtr material, WallType wallType,
              float bouncinessCoefficient, Vector2D center, float radius,
              float theta1, float theta2)
        : Wall(material, wallType, bouncinessCoefficient),
          center(center),
          radius(radius),
          theta1(theta1),
          theta2(theta2) {}

    RoundWall(ReadPacket *rp);
    virtual void writeToPacket(WritePacket *wp) const;

    friend class PhysicsWorld;
    friend class UnholyGameViewController;
    friend class HolyGameViewController;
    friend class MultiGameViewController;
    friend class GameViewController;
};

class RoundObject : public Object {
protected:
    Vector2D center;
    float radius, heightRatio;

public:
    RoundObject(MaterialPtr material, Vector2D center, float radius,
                float heightRatio)
        : Object(material),
          center(center),
          radius(radius),
          heightRatio(heightRatio) {}

    RoundObject(ReadPacket *rp);
    virtual void writeToPacket(WritePacket *wp) const;

    friend class PhysicsWorld;
    friend class UnholyGameViewController;
    friend class HolyGameViewController;
    friend class MultiGameViewController;
    friend class GameViewController;
};

enum MovingObjectState {
    MOS_SPAWNING,
    MOS_ALIVE,
    MOS_SHRINKING,
    MOS_DEAD,

    MOS_NUM_STATES
};

class MovingRoundObject : public RoundObject {
private:
    // Make object non-copyable by making this private.
    MovingRoundObject(MovingRoundObject const &);
    MovingRoundObject &operator=(MovingRoundObject const &);

    MovingObjectState state;

    float mass;

    // MOS_SPAWNING
    float timeUntilSpawn;

    // MOS_SHRINKING
    std::shared_ptr<MovingRoundObject> parent;
    int numChildren;

    int regionNumber;

    // Callbacks
    std::function<void()> onSpawnCallback;
    std::function<void()> onDeathCallback;

    Vector2D velocity;

    // helpers used by PhysicsWorld
    void startShrinking(std::shared_ptr<MovingRoundObject> parent, Vector2D const &velocity);
    void kill();

public:
    MovingRoundObject(MaterialPtr material, float radius, float mass,
                      float heightRatio, float timeUntilSpawn, int regionNumber,
                      std::function<void()> const &onSpawnCallback,
                      std::function<void()> const &onDeathCallback)
        : RoundObject(material, Vector2D(0.0, 0.0), radius, heightRatio),
          state(MOS_SPAWNING),
          mass(mass),
          timeUntilSpawn(timeUntilSpawn),
          regionNumber(regionNumber),
          onSpawnCallback(onSpawnCallback),
          onDeathCallback(onDeathCallback),
          velocity(0.0, 0.0) {}

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
    int getRegionNumber() const { return regionNumber; }

    /**
     * Gets the state.
     * @return the state.
     */
    MovingObjectState getState() const { return state; }

    /**
     * Gets the time until spawn. Should only be called if the
     * state is MOS_SPAWNING.
     * @return the time until spawn
     */
    float getTimeUntilSpawn() const { return timeUntilSpawn; }

    /**
     * Gets the parent for shrinking, or NULL if there is no
     * parent. Should only be called if the state is MOS_SHRINKING
     * @return a pointer to the parent Player
     */
    std::shared_ptr<MovingRoundObject> getShrinkingParent() const { return parent; }

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
    int getNumShrinkingChildren() const { return numChildren; }

    /**
     * @param velocity the new velocity for this object
     */
    void setVelocity(const Vector2D &velocity) {
        assert(state == MOS_ALIVE);
        this->velocity = velocity;
    }

    /**
     * @return the velocity
     */
    const Vector2D &getVelocity() const { return velocity; }

    /**
     * @return the mass
     */
    float getMass() const { return mass; }

    void instantKill() { state = MOS_DEAD; }

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
    virtual void writeToPacket(WritePacket *wp) const;

    friend class PhysicsWorld;
    friend class UnholyGameViewController;
    friend class HolyGameViewController;
    friend class MultiGameViewController;
    friend class GameViewController;
};

#endif
