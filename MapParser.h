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
        void parseTeams();
        void parseSpawns();
        void parseFlags();
        void parseWalls();

        std::ifstream in;
        std::string mapName;
        unsigned width, height;
        std::vector<Mode> modes;
};

#endif

