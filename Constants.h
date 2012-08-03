#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>
#include <cstdint>

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
const float FOUR_BY_THREE = 1.33333f;
const float SIXTEEN_BY_TEN = 1.6f;
const float SIXTEEN_BY_NINE = 1.777777f;

const uint16_t fourbythree[][2] = {
	{640, 480},
	{800, 600},
	{1024, 768},
	{1280, 960},
	{1400, 1050},
	{1600, 1200},
	{2048, 1536}
};
const uint16_t sixteenbyten[][2] = {
	{800, 500},
	{1024, 640},
	{1280, 800},
	{1440, 900},
	{1680, 1050},
	{1920, 1200},
	{2560, 1600},
	{3840, 2400}
};
const uint16_t sixteenbynine[][2] = {
	{854, 480},
	{1024, 576},
	{1280, 720},
	{1366, 768},
	{1600, 900},
	{1920, 1080},
	{2560, 1440}
};

#define ALIGNMENT 0x10
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#endif
