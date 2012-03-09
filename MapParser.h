#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>

#include "hack.h"

class MapParser {
    public:
        MapParser();
        MapParser(std::string filename);
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

