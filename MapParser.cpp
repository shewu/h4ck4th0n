#include <exception>

#include "MapParser.h"

class ParseException : public exception {

};

MapParser::MapParser() {
    loadFromFile("map.hbm");
};

MapParser::MapParser(std::String filename) {
    loadFromFile(filename);
}

void MapParser::loadFromFile(std::String filename) {
    in = ifstream(filename);
    parseMapName();
    parseDimensions();
    parseTeams();
    parseSpawns();
    parseFlags();
    parseWalls();
}

void MapParser::parseMode() {

}

void MapParser::parseDimensions() {

}

void MapParser::parseTeams() {

}

void MapParser::parseSpawns() {

}

void MapParser::parseFlags() {

}

void MapParser::parseWalls() {

}

