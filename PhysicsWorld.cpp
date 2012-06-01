#include "Physics.h"
using namespace std;

FlagObject *addFlagObject(int teamNumber, vector<SpawnDescript> *possibleSpawns) {
	FlagObject *obj = new FlagObject(teamNumber, SPAWN_TIME);
	movingRoundObjects.insert(pair<int, MovingRoundObject*>(obj->getID(), obj));
	additions.push_back(pair<Object*,ObjectType>(obj, OT_FLAG_OBJECT));
}

PlayerObject *addPlayerObject(int teamNumber, vector<SpawnDescript> *possibleSpawns) {
	PlayerObject *obj = new PlayerObject(teamNumber, SPAWN_TIME);
	movingRoundObjects.insert(pair<int, MovingRoundObject*>(obj->getID(), obj));
	additions.push_back(pair<int,ObjectType>(obj->getID(), OT_PLAYER_OBJECT));
}

void removeDeadObjects() {
	map<int, MovingRoundObject*>::iterator iter = movingRoundObjects.begin();
	while(iter != movingRoundObjects.end()) {
		map<int, MovingRoundObject*>::iterator iter2 = iter;
		++iter2;

		MovingRoundObject *obj = iter->second;
		if (obj->getState() == MOS_DEAD) {
			movingRoundObjects.erase(iter);
			deletions.push_back(obj->getID());
			delete obj;
		}

		iter = iter2;
	}
}

void writeUpdates(WritePacket *wp) {
	for(int i = 0; i < additions.size(); i++) {
		wp->write_char((char) DT_ADD);
		wp->write_char((char) additions[i].second);
		additions[i].first->writeToPacket(wp);
	}
	additions.clear();

	for(int i = 0; i < deletions.size(); i++) {
		wp->write_char((char) DT_REMOVE);
		wp->write_int(deletions[i]);
	}
	deletions.clear();

	for(map<int, MovingRoundObject*>::iterator iter = movingRoundObjects.begin();
	              iter != movingRoundObjects.end(); ++iter) {
		MovingRoundObject *obj = iter->second;
		if(obj->hasDiff()) {
			wp->write_char((char) DT_UPDATE);
			wp->write_int(obj->getID());
			obj->writeDiff(wp);
		}
	}
}
