#ifndef MAPINFO_CTF_H
#define MAPINFO_CTF_H

#include <map>
#include <string>

enum CTFWallType {
	CTF_WT_NORMAL,
	CTF_WT_DEADLY,
	CTF_WT_GOAL1,
	CTF_WT_GOAL2,

	CTF_WT_NUMTYPES
};

static const std::map<std::string, char> CTFWTMap = {
	{"normal", CTF_WT_NORMAL},
	{"deadly", CTF_WT_DEADLY},
	{"goal1", CTF_WT_GOAL1},
	{"goal2", CTF_WT_GOAL2}
};

#endif
