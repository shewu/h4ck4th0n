#include <list>
#include <cstring>
#include <cstdlib>
#include <exception>
#include <stdexcept>

#include "HBMap.h"
#include "Exceptions.h"
#include "Object.h"

using std::list;
using std::map;
using std::string;

class StringTokenizer {
    public:
        StringTokenizer(string const& s, string delim = " \t") {
            init(s, delim);
        }
        // Fetches and removes the next token. If no tokens remain before the
        // operation, then throws an exception.
        string nextToken() {
            if (!hasMoreTokens()) {
                throw std::out_of_range("StringTokenizer has no more tokens!");
            }
            string ret = toks.front();
            toks.pop_front();
            return ret;
        }
        bool hasMoreTokens() const {
            return toks.size() > 0;
        }
    private:
        list<string> toks;
        void init(string const& s, string delim) {
            const char* d = delim.c_str();
            char* pch = strtok(const_cast<char*>(s.c_str()), d);
            while (pch) {
                toks.push_back(string(pch));
                pch = strtok(NULL, d);
            }
        }
};

void HBMap::parse(string const& filename) {
    width = -1;
    height = -1;
    in.open(filename.c_str());
    string cmd;
    string s;
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
        } else if (cmd.compare("wall") == 0) {
            parseRectangularWall(s);
        }
        cmd = "";
    }
}

void HBMap::parseHBMapName(string const& s) {
    mapName = s.substr(5, s.size());
}

GameMode parseGameMode(string str) {
    for (int i = 0; i < NUM_GAMEMODES; ++i) {
        if (str.compare(modeStrings[i]) == 0) {
            return (GameMode)i;
        }
    }
    return GM_INVALID;
}

void HBMap::parseModes(string const& s) {
    StringTokenizer st(s);
    while (st.hasMoreTokens()) {
        modes.insert(parseGameMode(st.nextToken()));
    }
}

void HBMap::parseDimensions(string const& s) {
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

void HBMap::parseTeam(string const& s) {
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

    teams.push_back(TeamDescriptor(teamNum, minPlayers, maxPlayers));
}

void HBMap::parseSpawn(string const& s) {
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

    if (id < 0 || id >= (int) teams.size() || st.hasMoreTokens()) {
        throw ParseException("parseSpawn fail");
    }

    spawns[id].push_back(SpawnDescriptor(x, y, x+5, y+5));
}

WallTypeData HBMap::parseWallType(string const& s) {
	auto wallTypeMapIter = wallTypeDataLookup.find(gameType);
	if (wallTypeMapIter == wallTypeDataLookup.end()) {
		throw ParseException("game type not found in wallTypeDataLookup");
	}
	map<string, WallTypeData> const& wallTypeMap = wallTypeMapIter->second;
	auto wallTypeIter = wallTypeMap.find(s);
	if (wallTypeIter == wallTypeMap.end()) {
		throw ParseException("wall type `" + s + "' not recognized");
	}
	return wallTypeIter->second;
}

void HBMap::parseRectangularWall(string const& s) {
	int a = 0, b = 0, c = 0, d = 0;
    StringTokenizer st(s);
    WallTypeData wallTypeData;
    if (st.hasMoreTokens()) {
		string tok = st.nextToken();
        wallTypeData = parseWallType(tok);
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

	rectangularWalls.push_back(RectangularWallDescriptor(wallTypeData, Vector2D(a, b), Vector2D(c, d)));
}

