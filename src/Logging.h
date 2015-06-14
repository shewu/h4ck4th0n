#ifndef LOGGING_H
#define LOGGING_H

#include <fstream>

#include "Packet.h"

static const char kLoggingRemovePlayer = 0;
static const char kLoggingAddPlayer = 1;
static const char kLoggingProcessPacket = 2;
static const char kLoggingDoSimulation = 3;
static const char kLoggingDone = 4;
static const char kLoggingRandomSeed = 5;

void logRemovePlayer(std::fstream &f, int playerID);
void logAddPlayer(std::fstream &f, int playerID);
void logProcessPacket(std::fstream &f, int playerID, ReadPacket *rp);
void logDoSimulation(std::fstream &f, float dt);
void logDone(std::fstream &f);
void logRandomSeed(std::fstream &f, unsigned int seed);

#endif
