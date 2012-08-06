#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>
#include <cstdint>
#include <list>
#include <utility>

const float FRICTION = 0.3;
const float KEYPRESS_ACCELERATION = 6.0f;
const float DEATH_RATE = 0.05;
const float EPS = 0.000001;
const unsigned MAX_EVENTS = 10;
const float SPAWN_TIME = 3;
const float SPAWN_TRY = 0.1;
const float SPAWN_PROB = 0.1;
const unsigned MAX_CLIENTS = 32;

const int PROTMAGIC = 0xee76f6a8;
const unsigned MAXPACKET = 10240;
const unsigned MAXQUEUE = 10000;
const unsigned TIMEOUT = 5;

const unsigned ALSRCS = 32;

const unsigned AA = 1;

typedef enum {
	kHBNoView,
	kHBMainMenuView,
	kHBServerConnectView,
	kHBGameView
} HBViewMode;

const float CYL_ANGLE_DELTA = M_PI/90.;

const float FIVE_BY_FOUR = 1.25f;
const float FOUR_BY_THREE = 1.33333f;
const float SIXTEEN_BY_TEN = 1.6f;
const float SIXTEEN_BY_NINE = 1.777777f;

const std::list<std::pair<uint16_t, uint16_t> > RESOLUTIONS[] = {
	std::list<std::pair<uint16_t, uint16_t> > {
		std::pair<uint16_t, uint16_t>(1280, 1024),
	},
	std::list<std::pair<uint16_t, uint16_t> > {
		std::pair<uint16_t, uint16_t>(640, 480),
		std::pair<uint16_t, uint16_t>(800, 600),
		std::pair<uint16_t, uint16_t>(1024, 768),
		std::pair<uint16_t, uint16_t>(1280, 960),
		std::pair<uint16_t, uint16_t>(1440, 1050),
		std::pair<uint16_t, uint16_t>(1600, 1200),
		std::pair<uint16_t, uint16_t>(2048, 1536),
	},
	std::list<std::pair<uint16_t, uint16_t> > {
		std::pair<uint16_t, uint16_t>(1024, 640),
		std::pair<uint16_t, uint16_t>(1280, 800),
		std::pair<uint16_t, uint16_t>(1440, 900),
		std::pair<uint16_t, uint16_t>(1680, 1050),
		std::pair<uint16_t, uint16_t>(1920, 1200),
		std::pair<uint16_t, uint16_t>(2560, 1600),
		std::pair<uint16_t, uint16_t>(2880, 1800),
		std::pair<uint16_t, uint16_t>(3840, 2400),
	},
	std::list<std::pair<uint16_t, uint16_t> > {
		std::pair<uint16_t, uint16_t>(1024, 576),
		std::pair<uint16_t, uint16_t>(1280, 720),
		std::pair<uint16_t, uint16_t>(1366, 768),
		std::pair<uint16_t, uint16_t>(1600, 900),
		std::pair<uint16_t, uint16_t>(1920, 1080),
		std::pair<uint16_t, uint16_t>(2560, 1440),
	}
};

#define ALIGNMENT 0x10
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#endif
