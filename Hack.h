#ifndef HACK_H
#define HACK_H

#include <vector>
#include <cstdlib>

#include "Constants.h"
#include "Socket.h"
#include "Packet.h"
#include "Vector.h"
#include "Material.h"
#include "WallType.h"

#ifdef DEBUG
#define P(x) printf x
#else
#define P(x)
#endif

enum GameMode {
    GM_INVALID = -1,
    GM_TAG,
    GM_CTF,
    
    NUM_GAMEMODES // always the last mode
};

const std::string modeStrings[] = {
    "TAG", "CTF"
};

inline float random_uniform_float(float l, float h) {
	return rand() / float(RAND_MAX) * (h - l) + l;
}

#endif
