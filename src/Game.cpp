#include "Game.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using std::bind;
using std::make_pair;
using std::map;
using std::pair;
using std::placeholders::_1;
using std::placeholders::_2;

Game::Game(HBMap const &hbmap) : world_(hbmap) {
    world_.setRoundWallCollisionCallback(
        std::bind(&Game::roundWallCollision, this, _1, _2));
    world_.setRoundRoundCollisionCallback(
        std::bind(&Game::roundRoundCollision, this, _1, _2));
}

void Game::init() {
    onInit();
}

bool Game::addPlayer(int playerID, WritePacket &initInfoWP) {
    if (players.find(playerID) != players.end()) {
        return false;
    }

    GamePlayer *gp = onPlayerAdded();
    if (gp == NULL) {
        return false;
    }

    players.insert(make_pair(playerID, gp));

    // dumb handshake
    initInfoWP.write_float(gp->input.getTheta());
    world_.getMap().writeToPacket(initInfoWP);

    return true;
}

void Game::removePlayer(int playerID) {
    auto iter = players.find(playerID);
    if (iter == players.end()) {
        return;
    }
    GamePlayer *player = iter->second;

    onPlayerRemoved(player);

    delete player;
    players.erase(iter);
}

int Game::getObjectIDOf(int playerID) {
    auto iter = players.find(playerID);
    if (iter == players.end()) {
        return kNoPlayerExists;
    }
    GamePlayer *player = iter->second;

    if (!player->obj) {
        return kNoObjectExists;
    } else {
        return player->obj->getID();
    }
}

std::string Game::getScoreByPlayerID(int playerID) {
    auto iter = players.find(playerID);
    if (iter == players.end()) {
        return "";
    }
    GamePlayer *player = iter->second;
    return getScore(player);
}

void Game::applyForcesFromInput(float dt) {
    for (auto &iter : players) {
        GamePlayer *player = iter.second;
        if (player->obj && player->obj->getState() == MOS_ALIVE) {
            Vector2D accel = player->input.getAcceleration();
            player->obj->setVelocity(player->obj->getVelocity() + accel * dt);
        }
    }
}

void Game::processPacket(int playerID, ReadPacket *rp) {
    auto iter = players.find(playerID);
    if (iter == players.end()) {
        return;
    }
    GamePlayer *player = iter->second;

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
