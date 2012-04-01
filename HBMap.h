#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>
#include <set>

#include "Object.h"

class HBMap {
    public:
		/**
		 * Empty constructor that does nothing.
		 */
        HBMap() { }

		/**
		 * Creates an HBMap instance given the map filename.
		 *
		 * @throw ParseException when the parser encounters invalid syntax.
		 */
        HBMap(std::string filename) {
            parse(filename);
        }

		/**
		 * Gets the width of the map.
		 *
		 * @return the width.
		 */
        unsigned getWidth() {
            return width;
        }

		/**
		 * Gets the height of the map.
		 *
		 * @return the height.
		 */
        unsigned getHeight() {
            return height;
        }

		/**
		 * Gets the name of the map.
		 *
		 * @return the name.
		 */
        std::string& getName() {
            return mapName;
        }

		/**
		 * Gets the game modes of the map.
		 *
		 * @return the game modes in no particular order.
		 */
        std::vector<GameMode>& getGameModes() {
            return modes;
        }

		/**
		 * Gets the team list and properties of the map.
		 *
		 * @return the team list containing TeamDescriptor objects in no particular
		 * order.
		 */
        std::vector<TeamDescriptor>& getTeamDescriptors() {
            return teams;
        }

		/**
		 * Gets the spawns for a given team. If an invalid team number is
		 * requested, returns the spawns for the first team.
		 *
		 * @param team the team number for which to get spawns.
		 *
		 * @return the spawns for team.
		 */
        std::vector<Obstacle>& getSpawnsForTeam(unsigned team) {
            if (team >= teams.size()) {
                return spawns[0];
            }
            return spawns[team];
        }

		/**
		 * Gets the flags for a given team. If an invalid team number is
		 * requested, returns the spawns for the first team.
		 *
		 * @param team the team number for which to get flags.
		 *
		 * @return the flags for team.
		 */
        std::vector<Spawn>& getFlagsForTeam(unsigned team) {
            if (team >= teams.size()) {
                return flags[0];
            }
            return flags[team];
        }

		/**
		 * Gets the walls for the map.
		 *
		 * @return a list of walls.
		 */
        std::vector<Obstacle>& getWalls() {
            return walls;
        }

		/**
		 * The maximum number of teams allowed in maps.
		 */
        const static unsigned MAX_TEAMS = 10;
    private:
        void parse(std::string filename);
        void parseHBMapName(std::string& s);
        void parseModes(std::string& s);
        void parseDimensions(std::string& s);
        void parseTeam(std::string& s);
        void parseSpawn(std::string& s);
        void parseFlag(std::string& s);
        void parseWall(std::string& s);

        std::ifstream in;

        std::string mapName;
        unsigned width, height;

		std::set<GameMode> modes;
		std::vector<TeamDescriptor> teams;
		std::vector<RectangularWall> walls;
		std::vector<Flag> flags;
};

#endif
