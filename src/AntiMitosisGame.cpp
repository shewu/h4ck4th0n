#include "AntiMitosisGame.h"

#include <sstream>

using std::pair;
using std::string;
using std::stringstream;

// Four region types
// 1 is red team
// 2 is blue team
// 3 is red team flag
// 4 is blue team flag

void AntiMitosisGame::doGameLogic() {
}

RoundCollisionResult AntiMitosisGame::roundWallCollision(std::shared_ptr<MovingRoundObject> obj1,
                                                         std::shared_ptr<Wall> wall) {
    int rn = obj1->getRegionNumber();
    bool ans;
    if (wall->getWallType() == CTF_WT_DEADLY) {
        ans = (rn == 1 || rn == 2);
    } else if (wall->getWallType() == CTF_WT_GOAL1) {
        ans = (rn == 4);
        score[0]++;
    } else if (wall->getWallType() == CTF_WT_GOAL2) {
        ans = (rn == 3);
        score[1]++;
    } else {
        ans = false;
    }

    if (ans) {
        if (rn == 1 || rn == 2) {
            sounds.push_back(Sound(SOUND_SPLAT2, world_.getCollisionPoint()));
        } else {
            sounds.push_back(Sound(SOUND_DING, world_.getCollisionPoint()));
        }
    } else {
        sounds.push_back(Sound(SOUND_BOING2, world_.getCollisionPoint()));
    }

    return ans ? RoundCollisionResult::DEATH : RoundCollisionResult::NOTHING;
}

std::pair<RoundCollisionResult, RoundCollisionResult>
AntiMitosisGame::roundRoundCollision(std::shared_ptr<MovingRoundObject> obj1,
                                     std::shared_ptr<MovingRoundObject> obj2) {
    int rn1 = obj1->getRegionNumber();
    int rn2 = obj2->getRegionNumber();
    bool sameType = !((rn1 == 1 || rn1 == 2) ^ (rn2 == 1 || rn2 == 2));
    if (obj1->getState() == MOS_SHRINKING) {
        if (sameType) {
            sounds.push_back(Sound(SOUND_SPLAT2, world_.getCollisionPoint()));
        } else {
            sounds.push_back(Sound(SOUND_BOING2, world_.getCollisionPoint()));
        }
        return std::make_pair(RoundCollisionResult::NOTHING,
                              sameType ? RoundCollisionResult::DEATH
                                       : RoundCollisionResult::NOTHING);
    } else if (obj2->getState() == MOS_SHRINKING) {
        if (sameType) {
            sounds.push_back(Sound(SOUND_SPLAT2, world_.getCollisionPoint()));
        } else {
            sounds.push_back(Sound(SOUND_BOING2, world_.getCollisionPoint()));
        }
        return std::make_pair(sameType ? RoundCollisionResult::DEATH
                                       : RoundCollisionResult::NOTHING,
                              RoundCollisionResult::NOTHING);
    } else {
        sounds.push_back(Sound(SOUND_BOING2, world_.getCollisionPoint()));
        return std::make_pair(RoundCollisionResult::NOTHING,
                              RoundCollisionResult::NOTHING);
    }
}

void AntiMitosisGame::onInit() {
    createFlag(3);
    createFlag(4);
}

GamePlayer *AntiMitosisGame::onPlayerAdded() {
    GamePlayer *gp = new GamePlayer();

    int teamNum;
    if (numplayers[0] < numplayers[1]) {
        teamNum = 1;
    } else if (numplayers[0] > numplayers[1]) {
        teamNum = 2;
    } else {
        teamNum = (rand() % 2) + 1;
    }

    teamLookup[gp] = teamNum;

    createNewPlayer(teamNum, gp);
    numplayers[teamNum - 1]++;

    return gp;
}

void AntiMitosisGame::onPlayerRemoved(GamePlayer *player) {
    // remove the player's object from the game
    if (player->obj && (player->obj->getState() == MOS_ALIVE ||
                                 player->obj->getState() == MOS_SPAWNING)) {
        player->obj->instantKill();
    }
    numplayers[teamLookup[player] - 1]--;
    teamLookup.erase(player);
}

void AntiMitosisGame::createFlag(int regionNum) {
    world_.addFlagObject(regionNum, []() {},
                         std::bind(&AntiMitosisGame::createFlag, this, regionNum));
}

void AntiMitosisGame::createNewPlayer(int teamNum, GamePlayer *gp) {
    std::shared_ptr<MovingRoundObject> obj =
        world_.addPlayerObject(teamNum, []() {}, []() {});

    obj->setSpawnCallback([obj, gp]() { gp->obj = obj; });

    obj->setDeathCallback(
        std::bind(&AntiMitosisGame::createNewPlayer, this, teamNum, gp));
}

string AntiMitosisGame::getScore(GamePlayer *player) {
    stringstream ss;
    ss << "Red: " << score[0] << " Blue: " << score[1];
    return ss.str();
}
