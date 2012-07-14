#include "CTFGame.h"

using std::pair;

// Four region types
// 1 is red team
// 2 is blue team
// 3 is red team flag
// 4 is blue team flag

void CTFGame::doGameLogic() {
	// TODO compute scores and stuff
}

bool CTFGame::roundWallCollision(
		ObjectPtr<MovingRoundObject> obj1, ObjectPtr<RectangularWall> wall) {
	int rn = obj1->getRegionNumber();
	if (wall->getWallType() == WT_DEADLY) {
		return rn == 1 || rn == 2;
	} else if (wall->getWallType() == WT_GOAL) {
		return rn == 3 || rn == 4;
	} else {
		return false;
	}
}
std::pair<bool, bool> CTFGame::roundRoundCollision(
		ObjectPtr<MovingRoundObject> obj1, ObjectPtr<MovingRoundObject> obj2) {
	int rn1 = obj1->getRegionNumber();
	int rn2 = obj2->getRegionNumber();
	bool sameType = !( (rn1 == 1 || rn1 == 2) ^ (rn2 == 1 || rn2 == 2) );
	if (obj1->getState() == MOS_SHRINKING) {
		return pair<bool, bool>(false, sameType);
	} else if (obj2->getState() == MOS_SHRINKING) {
		return pair<bool, bool>(sameType, false);
	} else {
		return pair<bool, bool>(false, false);
	}
}
