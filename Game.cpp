#include "Game.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using std::map;
using std::pair;

bool Game::addPlayer(SocketConnection *sc) {
	if (players.find(sc) != players.end()) {
		return false;
	}

	players.insert(pair<SocketConnection*, GamePlayer>(
				sc, GamePlayer()));

	// TODO handshake

	return true;
}

void Game::applyForcesFromInput(float dt) {
	for (auto iter = players.begin(); iter != players.end(); ++iter) {
		GamePlayer& player = iter->second;
		if (!player.obj.empty()) {
			Vector2D accel = player.input.getAcceleration();
			player.obj->setVelocity(player.obj->getVelocity() + accel * dt);
		}
	}
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
