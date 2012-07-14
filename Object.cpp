#include "Object.h"

#include "Hack.h"
#include "Packet.h"

const float MovingRoundObject::kPlayerRadius = 1.0f;
const float MovingRoundObject::kPlayerMass = 1.0f;
const float MovingRoundObject::kPlayerHeightRatio = 1.0f;

const float MovingRoundObject::kFlagRadius = 1.0f;
const float MovingRoundObject::kFlagMass = 1.2f;
const float MovingRoundObject::kFlagHeightRatio = 2.0f;

unsigned Object::nextId = 0;

const MaterialPtr Object::materialsByTeamNumber[] = {MaterialPtr(new Color(0, 255, 0)),
                                                     MaterialPtr(new Color(255, 0, 0)),
                                                     MaterialPtr(new Color(0, 0, 255))
                                                    };

void Object::writeToPacket(WritePacket *wp) const {
	wp->write_int(id);
	material->writeToPacket(wp);
}

void RectangularObject::writeToPacket(WritePacket *wp) const {
	Object::writeToPacket(wp);
	wp->write_float(p1.x);
	wp->write_float(p1.y);
	wp->write_float(p2.x);
	wp->write_float(p2.y);
}

void RectangularWall::writeToPacket(WritePacket *wp) const {
	RectangularObject::writeToPacket(wp);
	wp->write_char((char) wallType);
}

void RoundObject::writeToPacket(WritePacket *wp) const {
	Object::writeToPacket(wp);
	wp->write_float(center.x);
	wp->write_float(center.y);
	wp->write_float(radius);
	wp->write_float(startAngle);
	wp->write_float(endAngle);
}

void RoundWall::writeToPacket(WritePacket *wp) const {
	RoundObject::writeToPacket(wp);
	wp->write_char((char) wallType);
}

void MovingRoundObject::writeToPacket(WritePacket *wp) const {
	RoundObject::writeToPacket(wp);
	wp->write_char((char) state);
	wp->write_float(velocity.x);
	wp->write_float(velocity.y);
	wp->write_float(mass);
	wp->write_float(heightRatio);
}

Object::Object(ReadPacket *rp) {
	id = rp->read_int();
	material.reset(Material::readFromPacket(rp));
}

RectangularObject::RectangularObject(ReadPacket *rp) : Object(rp) { 
	p1.x = rp->read_float();
	p1.y = rp->read_float();
	p2.x = rp->read_float();
	p2.y = rp->read_float();
}

RectangularWall::RectangularWall(ReadPacket *rp) : RectangularObject(rp) {
	wallType = (WallType) rp->read_char();
}

RoundObject::RoundObject(ReadPacket *rp) : Object(rp) {
	center.x = rp->read_float();
	center.y = rp->read_float();
	radius = rp->read_float();
	startAngle = rp->read_float();
	endAngle = rp->read_float();
}

RoundWall::RoundWall(ReadPacket *rp) : RoundObject(rp) {
	wallType = (WallType) rp->read_char();
}

MovingRoundObject::MovingRoundObject(ReadPacket *rp) : RoundObject(rp) {
	state = (MovingObjectState) rp->read_char();
	velocity.x = rp->read_float();
	velocity.y = rp->read_float();
	mass = rp->read_float();
	heightRatio = rp->read_float();

	parent = NULL;
	numChildren = 0;
}

void MovingRoundObject::startShrinking(MovingRoundObject *parent, Vector2D const& velocity) {
	this->parent = parent;
	this->state = MOS_SHRINKING;
	this->velocity = velocity;
	if(parent != NULL) {
		parent->numChildren++;
	}
}

void MovingRoundObject::kill() {
	state = MOS_DEAD;
	if(parent != NULL) {
		parent->numChildren--;
	}
}
