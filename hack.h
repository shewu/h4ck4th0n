#ifndef HACK_H
#define HACK_H

#include <map>
#include <vector>
#include "constants.h"
#include "socket.h"
#include "packet.h"

#ifdef DEBUG
#define P(x) printf x
#else
#define P(x)
#endif

enum {
    BLU,
    RED,
    NO_TEAM
};

enum GameMode {
    GM_INVALID = -1,
    GM_TAG,
    GM_CTF,
    
    NUM_GAMEMODES // always the last mode
};

std::string modeStrings[] = {
    "TAG", "CTF"
};

enum WallType {
    WT_INVALID = -1,
    WT_NORMAL,
    WT_DEADLY,
    WT_BOUNCY,

    NUM_WALLTYPES
};

std::string wallTypeStrings[] = {
    "normal", "deadly", "bouncy"
};

struct Team {
    int teamNumber;
    int minPlayers;
    int maxPlayers;
    Team() { }
    Team(int a, int b, int c) : teamNumber(a), minPlayers(b), maxPlayers(c) { }
};

class Vector2D
{
	public:
		float x, y;
		Vector2D(float a = 0, float b = 0) : x(a), y(b) {}
		Vector2D operator+(const Vector2D&) const;
		Vector2D operator-(const Vector2D&) const;
		Vector2D operator-() const;
		Vector2D& operator+=(const Vector2D&);
		Vector2D& operator-=(const Vector2D&);
		Vector2D& operator*=(const float);
		bool operator==(const Vector2D&) const;
		float operator*(const Vector2D&) const;
		Vector2D operator*(const float) const;
		Vector2D getNormalVector() const;
};

Vector2D operator*(const float, const Vector2D);

class Vector3D
{
	public:
		float x, y, z, w;
		Vector3D(float a = 0, float b = 0, float c = 0) : x(a), y(b), z(c), w(0) {}
		Vector3D(Vector2D& v) : x(v.x), y(v.y), z(0), w(0) {}
		Vector3D operator+(const Vector3D&) const;
		Vector3D operator-(const Vector3D&) const;
		Vector3D operator-() const;
		Vector3D& operator+=(const Vector3D&);
		Vector3D& operator-=(const Vector3D&);
		Vector3D& operator*=(const float);
		bool operator==(const Vector3D&) const;
		float operator*(const Vector3D) const;
		Vector3D operator*(const float) const;
};

Vector3D operator*(const float, const Vector3D);

class Color
{
	public:
		unsigned char r, g, b, a; // align, dammit
		Color(unsigned char x = 0, unsigned char y = 0, unsigned char z = 0) : r(x), g(y), b(z), a(0) {} 
};

class Object
{
	public:
		Vector2D p;
		Vector2D v;
		float mass;
		float rad;
		Color color;
		float hrat;
		int id;
		
		bool dead;
		bool stopped;
		int spawnl;
		int spawny;
		double time_until_spawn;
		int nattached;
		int attachedTo;
		int player;
		int flag;
		
		void write_data(WritePacket* wp);
		void read_data(ReadPacket* rp);
};

class Spawn {
	public:
		float xmin, xmax, ymin, ymax;
		float mass;
		float rad;
		Color color;
		float hrat;
};

class Obstacle
{
	public:
		Vector2D p1, p2;
		Color color;
		bool sticky;
		int flag;
        WallType wallType;
        Obstacle(Vector2D a, Vector2D b, Color c, WallType d, int e = NO_TEAM) 
            : p1(a), p2(b), color(c), wallType(d), flag(e) { }
        Obstacle(Vector2D a, Vector2D b, Color c, bool d = false, int e = NO_TEAM)
            : p1(a), p2(b), color(c), sticky(d), flag(e) {}
};

class Light
{
	public:
		Vector3D position;
		Color color;
};

class World
{
	private:
		std::string mapName;
        Color wall_color;
	public:
		World();
		int spawn(int spawnl, int player, int flag);
		
		float minX, maxX, minY, maxY;
		std::map<int, std::vector<Spawn> > spawns;
		std::map<int, Object> objects;
		std::vector<Light> lights;
		std::vector<Obstacle> obstacles;
		//std::vector<std::pair<char, Vector2D> > sounds;
		void doSimulation(float dt, std::vector<std::pair<char, Vector2D> >& sounds);
		
		void sendObjects(SocketConnection* sc, int obj);
		void receiveObjects(ReadPacket* rp, int& obj);
};

#endif
