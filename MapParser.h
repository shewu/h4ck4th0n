#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>

class MapParser {
    public:
        MapParser();
        MapParser(std::String filename);
    private:
        void loadFromFile(std::String filename);
        void parseMapName();
        void parseMode();
        void parseDimensions();
        void parseTeams();
        void parseSpawns();
        void parseFlags();
        void parseWalls();

        std::ifstream in;
};

#endif

