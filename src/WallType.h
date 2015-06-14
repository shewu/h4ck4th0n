#ifndef WALLTYPE_H
#define WALLTYPE_H

#include "Material.h"

typedef char WallType;

struct WallTypeData {
    WallTypeData() {}
    WallTypeData(WallType wallType, MaterialPtr material)
        : wallType(wallType), material(material) {}

    WallType wallType;
    MaterialPtr material;
};

#endif
