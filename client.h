#include "hack.h"
#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

extern World world;
extern int myId;
extern float angle;

const float FOUR_BY_THREE = 1.33333f;
const float SIXTEEN_BY_TEN = 1.6f;
const float SIXTEEN_BY_NINE = 1.777777f;

const uint16_t fourbythree[][2] = 
{
	{640, 480}, 
	{800, 600}, 
	{1024, 768}, 
	{1280, 960}, 
	{1400, 1050}, 
	{1600, 1200}, 
	{2048, 1536}
};
const uint16_t sixteenbyten[][2] = 
{
	{800, 500}, 
	{1024, 640}, 
	{1280, 800}, 
	{1440, 900}, 
	{1680, 1050}, 
	{1920, 1200}, 
	{2560, 1600}, 
	{3840, 2400}
};
const uint16_t sixteenbynine[][2] = 
{
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
