#include <netinet/in.h>
#include <stdio.h>

#include "hack.h"
#include "Object.h"
#include "packet.h"

/*
void Object::write_data(WritePacket* wp) {
    wp->write_float(p.x);
    wp->write_float(p.y);
    wp->write_float(v.x);
    wp->write_float(v.y);
    wp->write_float(mass);
    wp->write_float(rad);

    wp->write_int(color.r);
    wp->write_int(color.g);
    wp->write_int(color.b);

    wp->write_float(hrat);
    wp->write_int(id);
}

void Object::read_data(ReadPacket *rp) {
    p.x = rp->read_float();
    p.y = rp->read_float();
    v.x = rp->read_float();
    v.y = rp->read_float();
    mass = rp->read_float();
    rad = rp->read_float();

    color.r = rp->read_int();
    color.g = rp->read_int();
    color.b = rp->read_int();

    hrat = rp->read_float();
    id = rp->read_int();
}
*/

void RectangularWall::writeToPacket(WritePacket* wp) {
	// TODO
}

RectangularWall RectangularWall::readFromPacket(ReadPacket *rp) {
	// TODO
}

void RoundWall::writeToPacket(WritePacket* wp) {
	// TODO
}

RoundWall RoundWall::readFromPacket(ReadPacket *rp) {
	// TODO
}

void FlagObject::writeToPacket(WritePacket* wp) {
	// TODO
}

FlagObject FlagObject::readFromPacket(ReadPacket *rp) {
	// TODO
}

void PlayerObject::writeToPacket(WritePacket* wp) {
	// TODO
}

PlayerObject PlayerObject::readFromPacket(ReadPacket *rp) {
	// TODO
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
