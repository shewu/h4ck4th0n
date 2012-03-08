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
    in = ifstream(filename);
    parseMapName();
    parseMode();
    parseDimensions();
    parseTeams();
    parseSpawns();
    parseFlags();
    parseWalls();
}

void MapParser::parseMapName() {
    string s;
    getline(in, s);
    if ((s = s.substr(0, 4)).compare("name") != 0) {
        throw new ParseException();
    }
    mapName = s.substr(1, s.length());
}

void MapParser::parseMode() {
    string s;
    getline(in, s);
    if ((s = s.substr(0, 4)).compare("mode") != 0) {
        throw new ParseException();
    }
    char* pch;
    // get 1st mode
    if ((pch = strtok(s.c_str(), " \t"))) {
        ;
    } else {
        // we should have at least one mode.
        throw new ParseException();
    }
    // get the rest of the modes
    while ((pch = strtok(NULL, " \t"))) {
        ;
    }
}

void MapParser::parseDimensions() {
    string s;
    getline(in, s);
    if ((s = s.substr(0, 3)).compare("dim") != 0) {
        throw new ParseException();
    }
    char* pch;
    // get width
    if ((pch = strtok(s.c_str(), " \t"))) {
        width = atoi(pch);
        pch = strtok(NULL, " \t");
    }
    // get height
    if ((pch = strtok(NULL, " \t"))) {
        height = atoi(pch);
        pch = strtok(NULL, " \t");
    }
    // no characters should be left
    if (pch) {
        throw new ParseException();
    }
}

void MapParser::parseTeams() {
    string s;
    getline(in, s);
    if ((s = s.substr(0, 4)).compare("team") != 0) {
        throw new ParseException();
    }
}

void MapParser::parseSpawns() {

}

void MapParser::parseFlags() {

}

void MapParser::parseWalls() {

}

