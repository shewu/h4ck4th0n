#ifndef CTFGAME_H
#define CTFGAME_H

#include "Game.h"
#include "mapinfo/CTF.h"

class CTFGame : public Game {

	public:

		CTFGame(HBMap const& hbmap) : Game(hbmap) { }

	private:
		
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
