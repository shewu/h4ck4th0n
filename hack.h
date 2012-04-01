#ifndef HACK_H
#define HACK_H

#include <vector>

#include "constants.h"
#include "socket.h"
#include "packet.h"
#include "Vector.h"
#include "Material.h"

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

class SpawnDescriptor {
	private:
		float minX, maxX, minY, maxY;

	public:
		/**
		 * Creates a SpawnDescriptor whose rectangle has vertices (x1,y1)
		 * and (x2,y2).
		 * @param x1 The first coordinate of one of the corners of the rectangle.
		 * @param y1 The second coordinate of one of the corners of the rectangle.
		 * @param x2 The first coordinate of one of corners of the rectangle.
		 * @param y2 The second coordinate of one of the corners of the rectangle.
		 */
		SpawnDescriptor(float x1, float y1, float x2, float y2) {
			if(x1 < x2) {
				minX = x1;
				maxX = x2;
			} else {
				minX = x2;
				maxX = x1;
			}
			if(y1 < y2) {
				minY = y1;
				maxY = y2;
			} else {
				minY = y2;
				maxY = y1;
			}
		}

		/**
		 * @returns the minimum X-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMinX() const {
			return minX;
		}

		/**
		 * @returns the maximum X-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMaxX() const {
			return maxX;
		}

		/**
		 * @returns the minimum Y-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMinY() const {
			return minY;
		}

		/**
		 * @returns the maximum Y-coordinate of the SpawnDescriptor's rectangle.
		 */
		const float getMaxY() const {
			return maxY;
		}
};

class TeamDescriptor {
    private:
        unsigned teamNumber;
        unsigned minPlayers;
        unsigned maxPlayers;

        std::vector<SpawnDescriptor> spawns;

    public:
        /**
         * Empty team constructor. Does nothing.
         */
        TeamDescriptor() { }

        /**
         * Constructs a team given the team number, minimum number of players,
         * and maximum number of players.
         *
         * @param a the team number.
         * @param b the minimum number of players.
         * @param c the maximum number of players.
         */
        TeamDescriptor(unsigned a, unsigned b, unsigned c) :
            teamNumber(a), minPlayers(b), maxPlayers(c) { }

        /**
         * Gets the team number for this team.
         *
         * @return the team number.
         */
        const unsigned getTeamNumber() const {
            return teamNumber;
        }

        /**
         * Gets the minimum number of players for this team.
         *
         * @return the minimum number of players.
         */
        const unsigned getMinPlayers() const {
            return minPlayers;
        }

        /**
         * Gets the maximum number of players for this team.
         *
         * @return the maximum number of players.
         */
        const unsigned getMaxPlayers() const {
            return maxPlayers;
        }

        /**
         * Gets the spawn descriptors for this team.
         * @return the spawn descriptors.
         */
        const std::vector<SpawnDescriptor>& getSpawnDescriptors() const {
        	return spawns;
		}
};

#endif

