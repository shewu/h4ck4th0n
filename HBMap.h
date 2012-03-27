#ifndef MAP_PARSER
#define MAP_PARSER

#include <fstream>
#include <vector>

#include "hack.h"

class HBMap {
    public:
        HBMap() { }
        HBMap(std::string filename) {
            parse(filename);
        }
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
        std::vector<Obstacle>& getSpawnsForTeam(unsigned team) {
            if (team >= teams.size()) {
                return spawns[0];
            }
            return spawns[team];
        }
        std::vector<Spawn>& getFlagsForTeam(unsigned team) {
            if (team >= teams.size()) {
                return flags[0];
            }
            return flags[team];
        }
        std::vector<Obstacle>& getWalls() {
            return walls;
        }
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
        std::vector<GameMode> modes;
        std::vector<Team> teams;
        std::vector<Obstacle> spawns[HBMap::MAX_TEAMS];
        std::vector<Spawn> flags[HBMap::MAX_TEAMS];
        std::vector<Obstacle> walls;
};

#endif

