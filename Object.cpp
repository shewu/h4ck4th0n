#include "Object.h"

#include "Hack.h"
#include "Packet.h"

float PlayerObject::PLAYER_RADIUS = 1;
float PlayerObject::PLAYER_MASS = 1;
float PlayerObject::PLAYER_HEIGHT_RATIO = 1;

float FlagObject::FLAG_RADIUS = 1.0f;
float FlagObject::FLAG_MASS = 1.2f;
float FlagObject::FLAG_HEIGHT_RATIO = 2.0f;

unsigned Object::nextId = 0;

Material* Object::materialsByTeamNumber[] = {new Color(0, 255, 0),
                                             new Color(255, 0, 0),
                                             new Color(0, 0, 255)
                                            };


ObjectPtr::ObjectPtr(Object* obj) : ptr_(obj) {
	obj->refCount++;
}

ObjectPtr::~ObjectPtr() {
	ptr_->refCount--;
}

ObjectPtr::ObjectPtr(ObjectPtr const& other) {
	ptr_ = other.ptr_;
	ptr_->refCount++;
}

ObjectPtr& ObjectPtr::operator=(ObjectPtr const& other) {
	ptr_->refCount--;
	ptr_ = other.ptr_;
	ptr_->refCount++;
}

Object& ObjectPtr::operator*() {
	return *ptr_;
}

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
	wp->write_char((char) isFlag);
}

void FlagObject::writeToPacket(WritePacket *wp) const {
	MovingRoundObject::writeToPacket(wp);
	wp->write_int(teamNumber);
}

void PlayerObject::writeToPacket(WritePacket *wp) const {
	MovingRoundObject::writeToPacket(wp);
	wp->write_int(teamNumber);
}

Object::Object(ReadPacket *rp) {
	id = rp->read_int();
	material = Material::readFromPacket(rp);
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
	isFlag = (bool) rp->read_char();

	parent = NULL;
	numChildren = 0;
}

FlagObject::FlagObject(ReadPacket *rp) : MovingRoundObject(rp) {
	teamNumber = rp->read_int();
}

PlayerObject::PlayerObject(ReadPacket *rp) : MovingRoundObject(rp) {
	teamNumber = rp->read_int();
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

bool MovingRoundObject::shouldDieFromWall(RectangularWall const& wall) const {
	if(isFlag) {
		return wall.getWallType() == WT_GOAL;
	} else {
		return wall.getWallType() == WT_DEADLY;
	}
}
