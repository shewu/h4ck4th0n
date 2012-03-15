#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>

#include "hack.h"

class MapParser {
    public:
        MapParser(std::string filename);
        unsigned getWidth() {
            return width;
        }
        unsigned getHeight() {
            return height;
        }
        std::string& getName() {
            return mapName;
        }
        std::vector<GameMode>& getGameModes() {
            return modes;
        }
        std::vector<Team>& getTeams() {
            return teams;
        }
        std::vector<Spawn>& getSpawns() {
            return spawns;
        }
        std::vector<Obstacle>& getWalls() {
            return walls;
        }
    private:
        void loadFromFile(std::string filename);
        void parseMapName();
        void parseModes();
        void parseDimensions();

        std::string& parseTeams();
        std::string& parseSpawns();
        std::string& parseFlags();
        std::string& parseWalls();

        std::ifstream in;
        std::string mapName;
        unsigned width, height;
        std::vector<GameMode> modes;
        std::vector<Team> teams;
        std::vector<Spawn> spawns;
        std::vector<Obstacle> walls;
};

#endif

