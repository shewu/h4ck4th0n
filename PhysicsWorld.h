#include "World.h"

#include <vector>

class PhysicsWorld : World
{
	public:
		PhysicsWorld(HBMap const& hbmap) : World(hbmap) { }

		void doSimulation(float dt);

		FlagObject* addFlagObject(int teamNumber,
		    std::vector<SpawnDescriptor> *possibleSpawns);

		PlayerObject* addPlayerObject(int teamNumber,
		    std::vector<SpawnDescriptor> *possibleSpawns);

		void removeDeadObjects();

		void writeToPacket(WritePacket *wp) const;

	private:
		enum EventType
		{
			ET_NONE,
			ET_ROUND_ROUND,
			ET_ROUND_DISAPPEAR,
			ET_ROUND_WALL_CORNER_1,
			ET_ROUND_WALL_CORNER_2,
			ET_ROUND_WALL_LINE
		};

		struct collide_event
		{
			float time;
			enum EventType type;

			/*
			 * Depends on type.
			 * ET_ROUND_ROUND: IDs of round objects
			 * ET_ROUND_DISAPPEAR: t1 is the ID of the object, t2 is unused.
			 * ET_ROUND_WALL_*: t1 is the ID of the round object, t2 ID of the wall.
			 */
			int t1, t2;

			collide_event() { }
			collide_event(float time, EventType type, int t1, int t2) :
				time(time), type(type), t1(t1), t2(t2) { }

			bool operator==(collide_event const &a) const {
				return type == a.type && t1 == a.t1 && t2 == a.t2 && time == a.time;
			}
			bool operator!=(collide_event const &a) const {
				return !(operator==(a));
			}
			bool operator<(collide_event const& a) const {
				return (time > a.time);
			}
		};

		// Helper methods for physics (see Physics.cpp)
		static float collideCircles(Vector2D diff, Vector2D vel, float r, float rc);
		static void doObjectCollision(MovingRoundObject const& fo, MovingRoundObject const& so,
				std::map<std::pair<int,int>, collide_event>& collideRoundWithRound,
				std::priority_queue<collide_event>& collideEvents,
				float cur, float dt);
		static void doRectangularWallCollision(MovingRoundObject const& obj,
				RectangularWall const& wall,
				std::map<std::pair<int, int>, collide_event>& collideRoundWithWall,
				std::priority_queue<collide_event>& collideEvents,
				float cur, float dt);
		static void doRoundObjectDisappearing(MovingRoundObject const& obj,
				std::map<int, collide_event>& collideDisappear,
				std::priority_queue<collide_event>& collideEvents,
				float cur, float dt);
		static void bounceMovingRoundAndShrinkingRound(MovingRoundObject& obj1,
				MovingRoundObject& obj2);
		static void bounceMovingRoundFromPoint(MovingRoundObject& obj, Vector2D const& p);
		static void bounceMovingRoundFromWall(MovingRoundObject& obj, Vector2D const& p1, Vector2D const& p2);
		static void updateRoundObjectsForward(std::map<int, MovingRoundObject*>& objects, float dt);
};
