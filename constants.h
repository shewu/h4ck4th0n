#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

#define FRICTION 0.3
#define KEYPRESS_ACCELERATION 6.0f
#define DEATH_RATE .05
#define MAX_DEPTH 200
#define EPS .000001
#define MAX_EVENTS 10
#define SPAWN_TIME 3
#define SPAWN_TRY .1
#define SPAWN_PROB .1
#define MAX_CLIENTS 32

#define PROTMAGIC 0xee76f6a8
#define MAXPACKET 10240
#define MAXQUEUE 10000
#define TIMEOUT 5

#define ALSRCS 32

#define AA 2

typedef enum {
	kHBNoView,
	kHBMainMenuView,
	kHBServerConnectView,
	kHBGameView
} HBViewMode;

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
