#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>

#include "hack.h"

class Map {
    public:
        Map(std::string filename);
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
        void parse(std::string filename);
        void parseMapName(std::string& s);
        void parseModes(std::string& s);
        void parseDimensions(std::string& s);
        void parseTeam(std::string& s);
        void parseSpawn(std::string& s);
        void parseFlag(std::string& s);
        void parseWall(std::string& s);

        std::ifstream in;
        std::string mapName;
        unsigned width, height;
        std::vector<GameMode> modes;
        std::vector<Team> teams;
        std::vector<Spawn> spawns;
        std::vector<Obstacle> walls;
};

#endif

