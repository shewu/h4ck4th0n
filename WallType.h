#ifndef WALLTYPE_H
#define WALLTYPE_H

#include "Material.h"

struct WallTypeData {
	WallTypeData() { }
	WallTypeData(char wallType, MaterialPtr material) :
		wallType(wallType),
		material(material) { }

	char wallType;
	MaterialPtr material;
};

#endif
