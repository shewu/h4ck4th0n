#ifndef MAPINFO_H
#define MAPINFO_H

#include <map>
#include <string>

#include "mapinfo/CTF.h"

static const std::map<std::string, std::map<std::string, char>> wallTypeLookup = {
	{"CTF", CTFWTMap}
};

#endif
