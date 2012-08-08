#include <list>
#include <cstring>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <utility>
#include <sstream>

#include "HBMap.h"
#include "Exceptions.h"
#include "Object.h"

using std::list;
using std::make_pair;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

// TODO not very safe code, there are probably some bad map files which would
// cause this to fail nongracefully

namespace {

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

	int string2int(string const& s) {
		stringstream ss(s);
		int ans;
		ss >> ans;
		if (ss.fail()) {
			throw ParseException("failure to parse int: " + s);
		}
		return ans;
	}

	float string2float(string const& s) {
		stringstream ss(s);
		float ans;
		ss >> ans;
		if (ss.fail()) {
			throw ParseException("failure to parse float: " + s);
		}
		return ans;
	}

	struct TokenizedLine {
		vector<string> args;
		map<string, string> attributes;

		TokenizedLine(string const& s) {
			StringTokenizer rest(s);
			while (rest.hasMoreTokens()) {
				string tok = rest.nextToken();
				int eqIndex = tok.find_first_of('=');
				if (eqIndex == string::npos) {
					args.push_back(tok);
				} else {
					attributes[tok.substr(0, eqIndex)] = tok.substr(eqIndex + 1);
				}
			}
		}
	};

} // anonymous namespace

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

		if (cmd == "name") {
			parseHBMapName(s);
		} else if (cmd == "mode") {
			parseModes(s);
		} else if (cmd == "dim") {
			parseDimensions(s);
		} else if (cmd == "spawn") {
			parseSpawn(s);
		} else if (cmd == "wall") {
			parseRectangularWall(s);
		} else if (cmd == "roundwall") {
			parseRoundWall(s);
		} else if (cmd == "floor") {
			parseFloor(s);
		} else {
			throw ParseException("unrecognized cmd '" + cmd + "'");
		}
		cmd = "";
	}
}

void HBMap::parseHBMapName(string const& s) {
	mapName = s.substr(5, s.size());
}

GameMode parseGameMode(string str) {
	for (int i = 0; i < NUM_GAMEMODES; ++i) {
		if (str == modeStrings[i]) {
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
	TokenizedLine tl(s);
	if (tl.args.size() != 2) {
		throw ParseException("failure parsing dimension: 2 arguments expected\n");
	}
	width = (unsigned) string2int(tl.args[0]);
	height = (unsigned) string2int(tl.args[1]);
}

void HBMap::parseSpawn(string const& s) {
	TokenizedLine tl(s);
	if (tl.args.size() != 3) {
		throw ParseException("failure parsing spawn: 3 arguments expected\n");
	}
	int id = string2int(tl.args[0]);
	int x = string2int(tl.args[1]);
	int y = string2int(tl.args[2]);
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
	TokenizedLine tl(s);
	if (tl.args.size() != 5) {
		throw ParseException("failure parsing rectangular wall: 5 arguments expected\n");
	}
	WallTypeData wallTypeData = parseWallType(tl.args[0]);
	int a = string2int(tl.args[1]);
	int b = string2int(tl.args[2]);
	int c = string2int(tl.args[3]);
	int d = string2int(tl.args[4]);

	float bc = WallDescriptor::kDefaultBouncinessCoefficient;
	if (tl.attributes["bounciness"] != "") {
		bc = string2float(tl.attributes["bounciness"]);
	}

	rectangularWalls.push_back(RectangularWallDescriptor(wallTypeData, Vector2D(a,b), Vector2D(c,d), bc));
}

void HBMap::parseRoundWall(string const& s) {
	TokenizedLine tl(s);
	if (tl.args.size() != 6) {
		throw ParseException("failure parsing rectangular wall: 6 arguments expected\n");
	}
	WallTypeData wallTypeData = parseWallType(tl.args[0]);
	int a = string2int(tl.args[1]);
	int b = string2int(tl.args[2]);
	int c = string2int(tl.args[3]);
	int d = string2int(tl.args[4]);
	int e = string2int(tl.args[5]);

	d %= 360;
	if (d < 0) {
		d += 360;
	}

	e %= 360;
	if (e < 0) {
		e += 360;
	}

	float bc = WallDescriptor::kDefaultBouncinessCoefficient;
	if (tl.attributes["bounciness"] != "") {
		bc = string2float(tl.attributes["bounciness"]);
	}

	roundWalls.push_back(RoundWallDescriptor(wallTypeData, Vector2D(a, b), c, float(d) * M_PI / 180., float(e) * M_PI / 180., bc));
}

void HBMap::parseFloor(string const& s) {
	TokenizedLine tl(s);
	if (tl.args.size() != 5) {
		throw ParseException("failure parsing rectangular wall: 5 arguments expected\n");
	}
	int a = string2int(tl.args[0]);
	int b = string2int(tl.args[1]);
	int c = string2int(tl.args[2]);
	int d = string2int(tl.args[3]);
	float e = string2float(tl.args[4]);
	floors.push_back(Floor(a,b,c,d,e));
}
