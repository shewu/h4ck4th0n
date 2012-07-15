#ifndef CTFGAME_H
#define CTFGAME_H

#include "Game.h"

class CTFGame : public Game {

	public:

		CTFGame(HBMap const& hbmap) : Game(hbmap) { }

	private:
		
		void doGameLogic();
		void handleCollision(ObjectPtr<Object>, ObjectPtr<Object>);

        bool roundWallCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<RectangularWall>);
        std::pair<bool, bool> roundRoundCollision(
        	ObjectPtr<MovingRoundObject>, ObjectPtr<MovingRoundObject>);
        GamePlayer* onPlayerAdded();
        void onPlayerRemoved(GamePlayer*);
        void onInit();

		void createFlag(int regionNum);
		void createNewPlayer(int teamNum, GamePlayer* gp);

};

#endif
