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
        HBMap(std::string const filename) {
            parse(filename);
        }

		/**
		 * Gets the width of the map.
		 *
		 * @return the width.
		 */
        const unsigned getWidth() const {
            return width;
        }

		/**
		 * Gets the height of the map.
		 *
		 * @return the height.
		 */
        const unsigned getHeight() const {
            return height;
        }

		/**
		 * Gets the name of the map.
		 *
		 * @return the name.
		 */
        const std::string& getName() const {
            return mapName;
        }

		/**
		 * Gets the game modes of the map.
		 *
		 * @return the game modes in no particular order.
		 */
        const std::set<GameMode>& getGameModes() const {
            return modes;
        }

		/**
		 * Gets the team list and properties of the map.
		 *
		 * @return the team list containing TeamDescriptor objects in no particular
		 * order.
		 */
        const std::vector<TeamDescriptor> getTeamDescriptors() const {
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
        const std::vector<SpawnDescriptor> getSpawnsForTeam(unsigned team) const {
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
        const std::vector<FlagObject>& getFlagsForTeam(unsigned team) const {
            if (team >= teams.size()) {
                return flags[0];
            }
            return flags[team];
        }

		/**
		 * Gets the rectangular walls for the map.
		 *
		 * @return a list of rectangular walls.
		 */
        const std::vector<RectangularWall>& getRectangularWalls() const {
            return rectangularWalls;
        }

		/**
		 * Gets the round walls for the map.
		 *
		 * @return a list of round walls.
		 */
		const std::vector<RoundWall>& getRoundWalls() const {
			return roundWalls;
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
		std::vector<RectangularWall> rectangularWalls;
		std::vector<RoundWall> roundWalls;
		std::vector<FlagObject> flags[HBMap::MAX_TEAMS];
        std::vector<SpawnDescriptor> spawns[HBMap::MAX_TEAMS];

        Color getColorForTeam(int team) {
            int colorFrac = int(255*float(team)/teams.size());
            return Color(colorFrac, 0, 255-colorFrac);
        }
};

#endif
