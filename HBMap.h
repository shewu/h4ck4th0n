#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>
#include <set>

#include "MapElements.h"
#include "Util.h"

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
			rectangularWalls = map.getRectangularWalls();
			roundWalls = map.getRoundWalls();
			for (int i = 0; i <= kMaxSpawnRegionNumber; ++i) {
				spawns[i] = map.getSpawnByNumber(i);
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
		 * Gets the spawns for a given team. If an invalid team number is
		 * requested, returns the spawns for the first team.
		 *
		 * @param team the team number for which to get spawns.
		 *
		 * @return the spawns for team.
		 */
		const SpawnDescriptor& getSpawnByNumber(unsigned regionNumber) const {
			if (regionNumber > kMaxSpawnRegionNumber) {
				P(("region number %d out of range\n", regionNumber));
				return spawns[0];
			}
			return spawns[regionNumber];
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
		 * The maximum spawn region number.
		 */
		const static int kMaxSpawnRegionNumber = 10;

	private:
		void parse(std::string const& filename);
		void parseHBMapName(std::string const& s);
		void parseModes(std::string const& s);
		void parseDimensions(std::string const& s);
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
		std::vector<RectangularWallDescriptor> rectangularWalls;
		std::vector<RoundWallDescriptor> roundWalls;
		SpawnDescriptor spawns[HBMap::kMaxSpawnRegionNumber + 1];
		std::vector<Floor> floors;
};

#endif
