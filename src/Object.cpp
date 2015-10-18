#include "Object.h"

#include "Packet.h"

const float MovingRoundObject::kPlayerRadius = 1.0f;
const float MovingRoundObject::kPlayerMass = 1.0f;
const float MovingRoundObject::kPlayerHeightRatio = 1.0f;

const float MovingRoundObject::kFlagRadius = 1.0f;
const float MovingRoundObject::kFlagMass = 1.2f;
const float MovingRoundObject::kFlagHeightRatio = 2.0f;

unsigned Object::nextId = 0;

// TODO fix materials
const MaterialPtr Object::materialsByTeamNumber[] = {
    MaterialPtr(new Color(0, 255, 0)), MaterialPtr(new Color(255, 0, 0)),
    MaterialPtr(new Color(0, 0, 255)), MaterialPtr(new Color(255, 0, 0)),
    MaterialPtr(new Color(0, 0, 255))};

void Object::writeToPacket(WritePacket *wp) const {
    wp->write_int(id);
    material->writeToPacket(wp);
}

void Wall::writeToPacket(WritePacket *wp) const {
    Object::writeToPacket(wp);

    assert(sizeof(wallType) == 1);
    wp->write_char(wallType);
}

void RectangularWall::writeToPacket(WritePacket *wp) const {
    Wall::writeToPacket(wp);
    wp->write_float(p1.x);
    wp->write_float(p1.y);
    wp->write_float(p2.x);
    wp->write_float(p2.y);
}

void RoundWall::writeToPacket(WritePacket *wp) const {
    Wall::writeToPacket(wp);
    wp->write_float(center.x);
    wp->write_float(center.y);
    wp->write_float(radius);
    wp->write_float(theta1);
    wp->write_float(theta2);
}

void RoundObject::writeToPacket(WritePacket *wp) const {
    Object::writeToPacket(wp);
    wp->write_float(center.x);
    wp->write_float(center.y);
    wp->write_float(radius);
    wp->write_float(heightRatio);
}

void MovingRoundObject::writeToPacket(WritePacket *wp) const {
    RoundObject::writeToPacket(wp);
    wp->write_float(mass);
    wp->write_float(velocity.x);
    wp->write_float(velocity.y);
    wp->write_char((char)state);
}

Object::Object(ReadPacket *rp) {
    id = rp->read_int();
    material.reset(Material::readFromPacket(rp));
}

Wall::Wall(ReadPacket *rp) : Object(rp) {
    wallType = rp->read_char();
}

RectangularWall::RectangularWall(ReadPacket *rp) : Wall(rp) {
    p1.x = rp->read_float();
    p1.y = rp->read_float();
    p2.x = rp->read_float();
    p2.y = rp->read_float();
}

RoundWall::RoundWall(ReadPacket *rp) : Wall(rp) {
    center.x = rp->read_float();
    center.y = rp->read_float();
    radius = rp->read_float();
    theta1 = rp->read_float();
    theta2 = rp->read_float();
}

RoundObject::RoundObject(ReadPacket *rp) : Object(rp) {
    center.x = rp->read_float();
    center.y = rp->read_float();
    radius = rp->read_float();
    heightRatio = rp->read_float();
}

MovingRoundObject::MovingRoundObject(ReadPacket *rp) : RoundObject(rp) {
    mass = rp->read_float();
    velocity.x = rp->read_float();
    velocity.y = rp->read_float();
    state = (MovingObjectState)rp->read_char();

    shrinkingParent = NULL;
    swallowerParent = NULL;
    numChildren = 0;
}

void MovingRoundObject::startShrinking(MovingRoundObject *parent,
                                       Vector2D const &velocity) {
    this->shrinkingParent = parent;
    this->numChildren = 0;
    this->state = MOS_SHRINKING;
    this->velocity = velocity;
    if (shrinkingParent != NULL) {
        shrinkingParent->numChildren++;
    }
    onDeathCallback();
}

void MovingRoundObject::startBeingSwallowed(MovingRoundObject *parent) {
    assert(parent != NULL);

    if (this->state == MOS_BEING_SWALLOWED) {
        this->swallowerParent->swallowees.erase(this);
    }

    this->swallowerParent = parent;
    this->state = MOS_BEING_SWALLOWED;
    this->swallowees.insert(this);
}

void MovingRoundObject::kill() {
    state = MOS_DEAD;
    if (shrinkingParent != NULL) {
        shrinkingParent->numChildren--;
    }
    if (swallowerParent != NULL) {
        swallowerParent->swallowees.erase(this);
    }
}
