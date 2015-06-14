#ifndef CTFGAME_H
#define CTFGAME_H

#include <map>

#include "Game.h"
#include "mapinfo/CTF.h"

class CTFGame : public Game {

	public:

		CTFGame(HBMap const& hbmap) : Game(hbmap) {
			numplayers[0] = 0;
			numplayers[1] = 0;
			score[0] = 0;
			score[1] = 0;
		}

	private:

		int numplayers[2];
		int score[2];
		std::map<GamePlayer*, int> teamLookup;
		
		void doGameLogic();

        bool roundWallCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<Wall>);
        std::pair<bool, bool> roundRoundCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<MovingRoundObject>);
        GamePlayer* onPlayerAdded();
        void onPlayerRemoved(GamePlayer*);
        void onInit();
        std::string getScore(GamePlayer*);

		void createFlag(int regionNum);
		void createNewPlayer(int teamNum, GamePlayer* gp);

};

#endif
