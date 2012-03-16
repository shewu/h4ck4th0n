#include <exception>
#include <list>
#include <cstring>

#include "Map.h"

class ParseException : public std::exception {
    public:
        ParseException();
};

class StringTokenizer {
    public:
        StringTokenizer(std::string& s, std::string delim = " \t") {
            init(s, delim);
        }
        // Fetches and removes the next token. If no tokens remain before the
        // operation, then throws an exception.
        std::string& nextToken() {
            if (!hasMoreTokens()) {
                throw new std::exception();
            }
            std::string ret = toks.front();
            ret.pop_front();
            return ret;
        }
        bool hasMoreTokens() {
            return toks.size() > 0;
        }
    private:
        std::list<string> toks;
        void init(std::string& s, std::string delim) {
            char* pch = strtok(s.c_str(), delim.c_str());
            while (pch) {
                toks.push_back(std::string(pch));
                pch = strtok(NULL, delim.c_str());
            }
        }
}

Map::Map(std::string filename = "map.hbm") {
    parse(filename);
}

void Map::parse(std::string filename) {
    width = -1;
    height = -1;
    in = ifstream(filename);
    std::string s;
    while (!in.eof()) {
        s = getline(in, s);
        if (s.substr(0, 4).compare("name") == 0) {
            parseMapName(s);
        } else if (s.substr(0, 4).compare("mode") == 0) {
            parseModes(s);
        } else if (s.substr(0, 3).compare("dim") == 0) {
            parseDimensions(s);
        } else if (s.substr(0, 4).compare("team") == 0) {
            parseTeam(s);
        } else if (s.substr(0, 5).compare("spawn") == 0) {
            parseSpawn(s);
        } else if (s.substr(0, 4).compare("flag") == 0) {
            parseFlag(s);
        } else if (s.substr(0, 4).compare("wall") == 0) {
            parseWall(s);
        } else {
            throw new ParseException();
        }
    }
}

void Map::parseMapName(std::string& s) {
    mapName = s.substr(5, s.size());
}

GameMode parseGameMode(std::string& str) {
    for (int i = 0; i < GameMode.NUM_MODES; ++i) {
        if (str.compare(modeStrings[i]) == 0) {
            return (GameMode)i;
        }
    }
    return -1;
}

void Map::parseModes(std::string& s) {
    s = s.substr(4, s.size());
    StringTokenizer st(s);
    while (st.hasMoreTokens()) {
        modes.push_back(parseGameMode(st.nextToken()));
    }
}

void Map::parseDimensions(std::string& s) {
    s = s.substr(3, s.size());
    StringTokenizer st(s);
    if (st.hasMoreTokens()) {
        width = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        height = atoi(st.nextToken().c_str());
    }
    if (width < 0 || height < 0 || st.hasMoreTokens()) {
        throw new ParseException();
    }
}

void Map::parseTeam(std::string& s) {
    s = s.substr(5, s.size());
    StringTokenizer st(s, " -\t");
    if (st.hasMoreTokens()) {
        teamNum = atoi(st.nextToken.c_str());
    }
    if (st.hasMoreTokens()) {
        minPlayers = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        maxPlayers = atoi(st.nextToken().c_str());
    }

    int teamNum = -1, minPlayers = -1, maxPlayers = -1;
    if (teamNum < 0 || minPlayers < 0 || maxPlayers < 0 || st.hasMoreTokens()) {
        throw new ParseException();
    }

    teams.push_back(Team(teamNum, minPlayers, maxPlayers));
}

void Map::parseSpawn(std::string& s) {
    s = s.substr(5, s.size());
    StringTokenizer st(s);
    int id = -1, x = -1, y = -1;
    if (st.hasMoreTokens()) {
        id = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        x = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        y = atoi(st.nextToken().c_str());
    }

    if (id < 0 || x < 0 || y < 0 || st.hasMoreTokens()) {
        throw new ParseException();
    }
}

void Map::parseFlag(std::string& s) {
    s = s.substr(4, s.size());
    StringTokenizer st(s);
    int id = -1, x = -1, y = -1;
    if (st.hasMoreTokens()) {
        id = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        x = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        y = atoi(st.nextToken().c_str());
    }

    if (id < 0 || x < 0 || y < 0 || st.hasMoreTokens()) {
        throw new ParseException();
    }
}

WallType parseWallType(std::string& s) {
    for (int i = 0; i < WallType.NUM_WALLTYPES; ++i) {
        if (modeStrings[i].compare(s) == 0) {
            return (WallType)i;
        }
    }
}

void Map::parseWall(std::string& s) {
    s = s.substr(4, s.size());
    StringTokenizer st(s);
    std::string wallType;
    int a = -1, b = -1, c = -1, d = -1;
    if (st.hasMoreTokens()) {
        wallType = st.nextToken();
    }
    if (st.hasMoreTokens()) {
        a = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        b = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        c = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        d = atoi(st.nextToken().c_str());
    }
    if (a < 0 || b < 0 || c < 0 || d < 0 || st.hasMoreTokens()) {
        throw new ParseException();
    }

    // assign wall colors later
    walls.push_back(Obstacle(Vector2D(a, b), Vector2D(c, d), Color(0, 0, 0), parseWallType(wallType)));
}

