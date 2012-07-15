#include "Game.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// TODO spawning

using std::bind;
using std::make_pair;
using std::map;
using std::pair;
using std::placeholders::_1;
using std::placeholders::_2;

Game::Game(HBMap const& hbmap) :
		world_(hbmap) {
	world_.setRoundWallCollisionCallback (
		std::bind(&Game::roundWallCollision,  this, _1, _2));
	world_.setRoundRoundCollisionCallback(
		std::bind(&Game::roundRoundCollision, this, _1, _2));
}

void Game::init() {
	onInit();
}

bool Game::addPlayer(SocketConnection* sc) {
	if (players.find(sc) != players.end()) {
		return false;
	}

	GamePlayer* gp = onPlayerAdded();
	if (gp == NULL) {
		return false;
	}

	players.insert(make_pair(sc, gp));

	// dumb handshake
	WritePacket wp(STC_INITIAL_ANGLE, 4);
	wp.write_float(gp->input.getTheta());
	sc->send_packet(wp);

	return true;
}

void Game::removePlayer(SocketConnection* sc) {
	auto iter = players.find(sc);
	if (iter == players.end()) {
		return;
	}
	GamePlayer* player = iter->second;

	if (!player->obj.empty() && player->obj->getState() == MOS_ALIVE) {
		player->obj->instantKill();
	}

	players.erase(iter);
}

int Game::getObjectIDOf(SocketConnection* sc) {
	auto iter = players.find(sc);
	if (iter == players.end()) {
		return kNoPlayerExists;
	}
	GamePlayer* player = iter->second;

	if (player->obj.empty()) {
		return kNoObjectExists;
	} else {
		return player->obj->getID();
	}
}

void Game::applyForcesFromInput(float dt) {
	for (auto iter = players.begin(); iter != players.end(); ++iter) {
		GamePlayer* player = iter->second;
		if (!player->obj.empty()) {
			Vector2D accel = player->input.getAcceleration();
			player->obj->setVelocity(player->obj->getVelocity() + accel * dt);
		}
	}
}

void Game::processPacket(SocketConnection* sc, ReadPacket* rp) {
	auto iter = players.find(sc);
	if (iter == players.end()) {
		return;
	}
	GamePlayer* player = iter->second;

	player->input = UserInput(rp);
}

void Game::update(float dt) {
	// apply forces
	applyForcesFromInput(dt);
	world_.applyForces(dt);

	// physics
	world_.doSimulation(dt);

	// game logic - compute score, etc.
	// any operation which is game-type specific
	doGameLogic();

	// clean up dead objects that Game does not have
	// any more pointers to
	world_.removeDeadObjects();
}
