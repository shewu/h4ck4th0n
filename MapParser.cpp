#include <exception>
#include <cstring>

#include "MapParser.h"

class ParseException : public exception {
    public:
        ParseException();
};

MapParser::MapParser() {
    loadFromFile("map.hbm");
};

MapParser::MapParser(std::string filename) {
    loadFromFile(filename);
}

void MapParser::loadFromFile(std::string filename) {
    width = -1;
    height = -1;
    in = ifstream(filename);
    parseMapName();
    parseModes();
    parseDimensions();

    // we don't know what line we're reading is for, so we will always read one
    // more line than necessary. pass that line to the next block of args to be
    // parsed.
    String s;
    parseTeams(s);
    parseSpawns(s);
    parseFlags(s);
    parseWalls(s);
}

void MapParser::parseMapName() {
    std::string s;
    getline(in, s);
    if ((s = s.substr(0, 4)).compare("name") != 0) {
        throw new ParseException();
    }
    mapName = s.substr(1, s.length());
}

GameMode parseGameMode(std::string& str) {
    for (int i = 0; i < GameMode.NUM_MODES; ++i) {
        if (str.compare(modeStrings[i]) == 0) {
            return (GameMode)i;
        }
    }
    return -1;
}

void MapParser::parseModes() {
    std::string s;
    getline(in, s);
    if ((s = s.substr(0, 4)).compare("mode") != 0) {
        throw new ParseException();
    }
    char* pch;
    // get 1st mode
    if ((pch = strtok(s.c_str(), " \t"))) {
        s = string(pch);
        modes.push_back(parseGameMode(s));
    } else {
        // we should have at least one mode.
        throw new ParseException();
    }
    // get the rest of the modes
    while ((pch = strtok(NULL, " \t"))) {
        s = string(pch);
        modes.push_back(parseMode(s));
    }
}

void MapParser::parseDimensions() {
    std::string s;
    getline(in, s);
    if ((s = s.substr(0, 3)).compare("dim") != 0) {
        throw new ParseException();
    }
    char* pch;
    // get width
    if ((pch = strtok(s.c_str(), " \t"))) {
        width = atoi(pch);
    }
    // get height
    if ((pch = strtok(NULL, " \t"))) {
        height = atoi(pch);
    }
    // no characters should be left
    if (width < 0 || height < 0 || strtok(NULL, " \t")) {
        throw new ParseException();
    }
}

std::string& MapParser::parseTeams(std::string& s) {
    while (1) {
        if (s.length() == 0) {
            getline(in, s);
        }
        if (s.substr(0, 4).compare("team")) {
            return s;
        }

        s = s.substr(0, 4);
        char* pch;
        int teamNum = -1, minPlayers = -1, maxPlayers = -1;
        if ((pch = strtok(s.c_str(), " -\t"))) {
            teamNum = atoi(pch);
        }
        if ((pch = strtok(NULL, " -\t"))) {
            minPlayers = atoi(pch);
        }
        if ((pch = strtok(NULL, " -\t"))) {
            maxPlayers = atoi(pch);
        }
        if (teamNum < 0 || minPlayers < 0 || maxPlayers < 0 || strtok(NULL, " -\t")) {
            throw new ParseException();
        }

        teams.push_back(Team(teamNum, minPlayers, maxPlayers));

        getline(in, s);
    }
}

std::string& MapParser::parseSpawns(std::string& s) {
    while (1) {
        if (s.substr(0, 5).compare("spawn")) {
            return s;
        }

        s = s.substr(0, 5);
        char* pch;
        if ((pch = strtok(pch, " \t"))) {
            ;
        }
        if ((pch = strtok(NULL, " \t"))) {
            ;
        }
        if ((pch = strtok(NULL, " \t"))) {
            ;
        }

        getline(in, s);
    }
}

std::string& MapParser::parseFlags(std::string& s) {
    while (1) {
        if (s.substr(0, 4).compare("flag")) {
            return s;
        }

        s = s.substr(0, 4);
        char* pch;
        if ((pch = strtok(pch, " \t"))) {
            ;
        }
        if ((pch = strtok(NULL, " \t"))) {
            ;
        }
        if ((pch = strtok(NULL, " \t"))) {
            ;
        }

        getline(in, s);
    }
}

WallType parseWallType(std::string& s) {
    for (int i = 0; i < WallType.NUM_WALLTYPES; ++i) {
        if (modeStrings[i].compare(s) == 0) {
            return (WallType)i;
        }
    }
}

std::string& MapParser::parseWalls(std::string& s) {
    while (1) {
        if (s.substr(0, 4).compare("wall")) {
            return s;
        }

        s = s.substr(0, 4);
        std::string wallType;
        int a = -1, b = -1, c = -1, d = -1;
        char* pch;
        if ((pch = strtok(NULL, " \t"))) {
            wallType = std::string(pch);
        }
        if ((pch = strtok(NULL, " \t"))) {
            a = atoi(pch);
        }
        if ((pch = strtok(NULL, " \t"))) {
            b = atoi(pch);
        }
        if ((pch = strtok(NULL, " \t"))) {
            c = atoi(pch);
        }
        if ((pch = strtok(NULL, " \t"))) {
            d = atoi(pch);
        }
        if (a < 0 || b < 0 || c < 0 || d < 0 || strtok(NULL, " \t")) {
            throw new ParseException();
        }

        // assign wall colors later
        walls.push_back(Obstacle(Vector2D(a, b), Vector2D(c, d), Color(0, 0, 0), parseWallType(wallType)));

        getline(in, s);
    }
}

