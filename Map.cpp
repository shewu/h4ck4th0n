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
        std::string nextToken() {
            if (!hasMoreTokens()) {
                throw new std::exception();
            }
            std::string ret = toks.front();
            toks.pop_front();
            return ret;
        }
        bool hasMoreTokens() const {
            return toks.size() > 0;
        }
    private:
        std::list<std::string> toks;
        void init(std::string& s, std::string delim) {
            const char* d = delim.c_str();
            char* pch = strtok(const_cast<char*>(s.c_str()), d);
            while (pch) {
                toks.push_back(std::string(pch));
                pch = strtok(NULL, d);
            }
        }
};

Map::Map() {
    parse("map.hbm");
}

Map::Map(std::string filename) {
    parse(filename);
}

void Map::parse(std::string filename) {
    width = -1;
    height = -1;
    in.open(filename.c_str());
    std::string cmd;
    std::string s;
    while (!in.eof()) {
        in >> cmd;
        getline(in, s);
        if (cmd.compare("name") == 0) {
            parseMapName(s);
        } else if (cmd.compare("mode") == 0) {
            parseModes(s);
        } else if (cmd.compare("dim") == 0) {
            parseDimensions(s);
        } else if (cmd.compare("team") == 0) {
            parseTeam(s);
        } else if (cmd.compare("spawn") == 0) {
            parseSpawn(s);
        } else if (cmd.compare("flag") == 0) {
            parseFlag(s);
        } else if (cmd.compare("wall") == 0) {
            parseWall(s);
        } else {
            throw new ParseException();
        }
    }
}

void Map::parseMapName(std::string& s) {
    mapName = s.substr(5, s.size());
}

GameMode parseGameMode(std::string str) {
    for (int i = 0; i < NUM_GAMEMODES; ++i) {
        if (str.compare(modeStrings[i]) == 0) {
            return (GameMode)i;
        }
    }
    return GM_INVALID;
}

void Map::parseModes(std::string& s) {
    StringTokenizer st(s);
    while (st.hasMoreTokens()) {
        modes.push_back(parseGameMode(st.nextToken()));
    }
}

void Map::parseDimensions(std::string& s) {
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
    StringTokenizer st(s, " -\t");
    int teamNum = -1, minPlayers = -1, maxPlayers = -1;
    if (st.hasMoreTokens()) {
        teamNum = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        minPlayers = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        maxPlayers = atoi(st.nextToken().c_str());
    }

    if (teamNum < 0 || minPlayers < 0 || maxPlayers < 0 || st.hasMoreTokens()) {
        throw new ParseException();
    }

    teams.push_back(Team(teamNum, minPlayers, maxPlayers));
}

void Map::parseSpawn(std::string& s) {
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

    //flags.push_back(Spawn(Vector2D(), Vector2D(), Color(), true, ));
}

WallType parseWallType(std::string& s) {
    for (int i = 0; i < NUM_WALLTYPES; ++i) {
        if (modeStrings[i].compare(s) == 0) {
            return (WallType)i;
        }
    }
    return WT_INVALID;
}

void Map::parseWall(std::string& s) {
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

