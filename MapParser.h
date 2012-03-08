#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>

class MapParser {
    public:
        MapParser();
        MapParser(std::string filename);
    private:
        void loadFromFile(std::string filename);
        void parseMapName();
        void parseMode();
        void parseDimensions();
        void parseTeams();
        void parseSpawns();
        void parseFlags();
        void parseWalls();

        std::ifstream in;
        std::string mapName;
        unsigned width, height;
};

#endif

