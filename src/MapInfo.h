#ifndef MAPINFO_H
#define MAPINFO_H

#include "WallType.h"
#include "mapinfo/CTF.h"

static const std::map<std::string, std::map<std::string, WallTypeData>>
    wallTypeDataLookup = {{"CTF", CTFWTMap}};

#endif
