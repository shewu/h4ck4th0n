#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "World.h"

#include <functional>
#include <vector>

class PhysicsWorld : World
{
	public:
		typedef std::function<bool(ObjectPtr<MovingRoundObject>,
		                           ObjectPtr<RectangularWall>
		                  )> RoundWallCollisionCallback;
		typedef std::function<std::pair<bool,bool>(ObjectPtr<MovingRoundObject>,
		                                           ObjectPtr<RectangularWall>
		                  )> RoundRoundCollisionCallback;

		PhysicsWorld(HBMap const& hbmap) : World(hbmap) {
			roundWallCollisionCallback = [](ObjectPtr<MovingRoundObject> a, ObjectPtr<RectangularWall> b) { return false; };
			roundRoundCollisionCallback = [](ObjectPtr<MovingRoundObject> a, ObjectPtr<MovingRoundObject> b) { return std::pair<bool,bool>(false,false); };
		}

		void applyForces(float dt);
		void doSimulation(float dt);

		/**
		 * These must be safe to call from a callback during a simulation
		 */
		ObjectPtr<MovingRoundObject> addFlagObject(int regionNumber,
			std::function<void()> const& onSpawnCallback,
			std::function<void()> const& onDeathCallback);
		ObjectPtr<MovingRoundObject> addPlayerObject(int regionNumber,
			std::function<void()> const& onSpawnCallback,
			std::function<void()> const& onDeathCallback);

		void removeDeadObjects();

		void writeToPacket(WritePacket *wp) const;

		void setRoundWallCollisionCallback(RoundWallCollisionCallback const& callback) {
			roundWallCollisionCallback = callback;
		}

		void setRoundRoundCollisionCallback(RoundRoundCollisionCallback const& callback) {
			roundRoundCollisionCallback = callback;
		}

	private:
		
		RoundWallCollisionCallback  roundWallCollisionCallback;
		RoundRoundCollisionCallback roundRoundCollisionCallback;

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
		static bool objectsIntersect(MovingRoundObject const& obj1, MovingRoundObject const& obj2);
		static bool objectsIntersect(MovingRoundObject const& obj, RectangularWall const& wall);
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
				MovingRoundObject& obj2, bool shouldDie);
		static void bounceMovingRoundFromPoint(MovingRoundObject& obj, Vector2D const& p);
		static void bounceMovingRoundFromWall(MovingRoundObject& obj, Vector2D const& p1, Vector2D const& p2);
		static void updateRoundObjectsForward(std::map<int, MovingRoundObject*>& objects, float dt);
};

#endif
