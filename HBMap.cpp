#include <list>
#include <cstring>
#include <cstdlib>
#include <exception>

#include "HBMap.h"
#include "Exceptions.h"

class StringTokenizer {
    public:
        StringTokenizer(std::string& s, std::string delim = " \t") {
            init(s, delim);
        }
        // Fetches and removes the next token. If no tokens remain before the
        // operation, then throws an exception.
        std::string nextToken() {
            if (!hasMoreTokens()) {
                throw std::out_of_range("StringTokenizer has no more tokens!");
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

void HBMap::parse(std::string filename) {
    width = -1;
    height = -1;
    in.open(filename.c_str());
    std::string cmd;
    std::string s;
    while (!in.eof()) {
        in >> cmd;
        // we need this check because checking for in.eof() doesn't work the way
        // we want.
        if (cmd.length() == 0) {
            break;
        }
        getline(in, s);

        // get rid of comments
        int commentStartPos = s.find_first_of('%');
        s = s.substr(0, commentStartPos);

        if (cmd.compare("name") == 0) {
            parseHBMapName(s);
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
        }
        cmd = "";
    }
}

void HBMap::parseHBMapName(std::string& s) {
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

void HBMap::parseModes(std::string& s) {
    StringTokenizer st(s);
    while (st.hasMoreTokens()) {
        modes.push_back(parseGameMode(st.nextToken()));
    }
}

void HBMap::parseDimensions(std::string& s) {
    StringTokenizer st(s);
    if (st.hasMoreTokens()) {
        width = atoi(st.nextToken().c_str());
    }
    if (st.hasMoreTokens()) {
        height = atoi(st.nextToken().c_str());
    }
    if (width < 0 || height < 0 || st.hasMoreTokens()) {
        throw ParseException("parseDimensions fail");
    }
}

void HBMap::parseTeam(std::string& s) {
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
        throw ParseException("parseTeam fail");
    }

    teams.push_back(Team(teamNum, minPlayers, maxPlayers));
}

void HBMap::parseSpawn(std::string& s) {
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

    if (id < 0 || id >= teams.size() || st.hasMoreTokens()) {
        throw ParseException("parseSpawn fail");
    }

    spawns[id].push_back(SpawnDescriptor(x, y, x+5, y+5));
}

void HBMap::parseFlag(std::string& s) {
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

    if (id < 0 || id >= teams.size() || st.hasMoreTokens()) {
        throw ParseException("parseFlag fail");
    }

    flags.push_back(Flag(getColorForTeam(id), Vector2D(x, y), id));
}

WallType parseWallType(std::string& s) {
    for (int i = 0; i < NUM_WALLTYPES; ++i) {
        if (wallTypeStrings[i].compare(s) == 0) {
            return (WallType)i;
        }
    }
    return WT_INVALID;
}

void HBMap::parseWall(std::string& s) {
    StringTokenizer st(s);
    WallType wallType;
    if (st.hasMoreTokens()) {
        wallType = parseWallType(st.nextToken());
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
    if (st.hasMoreTokens()) {
        throw ParseException("parseWall fail");
    }

    // assign wall colors later
    walls.push_back(Obstacle(Vector2D(a, b), Vector2D(c, d), Color(0, 0, 0), parseWallType(wallType)));
}

