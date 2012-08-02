#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>
#include <set>

#include "MapElements.h"
#include "MapInfo.h"

enum GameMode {
    GM_INVALID = -1,
    GM_TAG,
    GM_CTF,
    
    NUM_GAMEMODES // always the last mode
};

const std::string modeStrings[] = {
    "TAG", "CTF"
};

class HBMap {
    public:
		/**
		 * Empty constructor that does nothing.
		 */
        HBMap() { }

		/**
		 * Creates an HBMap instance given the map filename.
		 *
		 * @param filename the filename of the map file to be read.
		 *
		 * @throw ParseException when the parser encounters invalid syntax.
		 */
        HBMap(std::string const& filename, std::string const& gameType) {
            this->gameType = gameType;

            parse(filename);
        }

        /**
         * Creates an HBMap instance by reading from the ReadPacket
         */
        HBMap(ReadPacket *rp) {
			width = rp->read_int();
			height = rp->read_int();
		}

		void writeToPacket(WritePacket& wp) const {
			wp.write_int(width);
			wp.write_int(height);
		}
		// TODO read/write more info from these

		// TODO we should consider making map noncopyable
		/**
		 * Copy constructor for HBMap.
		 *
		 * @param map the HBMap to be copied.
		 */
		HBMap(HBMap const& map) {
			width = map.getWidth();
			height = map.getHeight();

			modes = map.getGameModes();
			teams = map.getTeamDescriptors();
			rectangularWalls = map.getRectangularWalls();
			roundWalls = map.getRoundWalls();
			for (int i = 0; i < (int) teams.size(); ++i) {
				spawns[i] = map.getSpawnsForTeam(i);
			}
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
        const std::vector<SpawnDescriptor>& getSpawnsForTeam(unsigned team) const {
            if (team >= teams.size()) {
                return spawns[0];
            }
            return spawns[team];
        }

		/**
		 * Gets the rectangular walls for the map.
		 *
		 * @return a list of rectangular walls.
		 */
        const std::vector<RectangularWallDescriptor>& getRectangularWalls() const {
            return rectangularWalls;
        }

		/**
		 * Gets the round walls for the map.
		 *
		 * @return a list of round walls.
		 */
		const std::vector<RoundWallDescriptor>& getRoundWalls() const {
			return roundWalls;
		}

		/**
		 * Gets the floors for the map.
		 *
		 * @return a vector of floors.
		 */
		 const std::vector<Floor>& getFloors() const {
		 	 return floors;
		 }

		/**
		 * The maximum number of teams allowed in maps.
		 */
        const static unsigned MAX_TEAMS = 10;

    private:
        void parse(std::string const& filename);
        void parseHBMapName(std::string const& s);
        void parseModes(std::string const& s);
        void parseDimensions(std::string const& s);
        void parseTeam(std::string const& s);
        void parseSpawn(std::string const& s);
        void parseRectangularWall(std::string const& s);
        void parseRoundWall(std::string const& s);
        void parseFloor(std::string const& s);
        WallTypeData parseWallType(std::string const& s);

        std::string gameType;

        std::ifstream in;

        std::string mapName;
        unsigned width, height;

		std::set<GameMode> modes;
		std::vector<TeamDescriptor> teams;
		std::vector<RectangularWallDescriptor> rectangularWalls;
		std::vector<RoundWallDescriptor> roundWalls;
        std::vector<SpawnDescriptor> spawns[HBMap::MAX_TEAMS];
        std::vector<Floor> floors;

        Color getColorForTeam(int team) {
            int colorFrac = int(255*float(team)/teams.size());
            return Color(colorFrac, 0, 255-colorFrac);
        }
};

#endif
