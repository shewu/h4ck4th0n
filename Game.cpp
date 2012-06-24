#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "Game.h"
#include "Constants.h"

Game::Game() {
	worldWritePacket_ = new WritePacket(STC_SEND_WORLD_STATE);
}

Game::~Game() {
	delete worldWritePacket_;
}

void Game::update(float dt) {
	// physics
	world_.doSimulation(dt);

	// game logic - compute score, etc.
	// any operation which is game-type specific
	doGameLogic();

	// clean up dead objects that Game does not have
	// any more pointers to
	world_.cleanUp();
}
