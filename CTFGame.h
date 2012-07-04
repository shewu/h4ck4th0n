#ifndef CTFGAME_H
#define CTFGAME_H

#include "Game.h"

class CTFGame : public Game {

	public:

		CTFGame(HBMap const& hbmap) : Game(hbmap) { }

	private:
		
		void doGameLogic();

};

#endif
