#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <vector>
#include <utility>

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

typedef enum {
	kHBNoView,
	kHBMainMenuView,
	kHBServerConnectView,
	kHBGameView
} HBViewMode;

const float kHBAspectRatioFiveByFour = 1.25f;
const float kHBAspectRatioFourByThree = 1.33333f;
const float kHBAspectRatioSixteenByTen = 1.6f;
const float kHBAspectRatioSixteenByNine = 1.777777f;

// who still uses a 17" or 19" square screen?
const std::vector<std::pair<int, int> > five_by_four = {
    std::make_pair(1280, 1024),
};

// who has a square screen???
const std::vector<std::pair<int, int> > four_by_three = {
    std::make_pair(640, 480),  // simple windowed mode
    std::make_pair(800, 600),
    std::make_pair(1024, 768),
    std::make_pair(1280, 960),
    std::make_pair(1400, 1050),
    std::make_pair(1600, 1200),
    std::make_pair(2048, 1536),
};

const std::vector<std::pair<int, int> > sixteen_by_ten = {
    std::make_pair(1280, 800),
    std::make_pair(1440, 900),
    std::make_pair(1680, 1050),
    std::make_pair(1920, 1200),
    std::make_pair(2560, 1600),
    std::make_pair(3840, 2400),  // sweet jesus i'd love to have an IBM T221
};

const std::vector<std::pair<int, int> > sixteen_by_nine = {
    std::make_pair(1280, 720),
    std::make_pair(1366, 768),
    std::make_pair(1600, 900),
    std::make_pair(1920, 1080),
    std::make_pair(2560, 1440),
    std::make_pair(3840, 2160),  // TODO buy a 4K display
};

#define ALIGNMENT 0x10
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#endif
