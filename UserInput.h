#ifndef USER_INPUT_H
#define USER_INPUT_H

#include "Vector.h"
#include "Packet.h"

/**
 * Represents a user input state.
 */
class UserInput {
	private:
		bool _left, _right, _up, _down;
		float _theta;

	public:
		/**
		 * Creates a UserInput object from the given input key inputs.
		 * @param left True if the `left' button is pressed down.
		 * @param right True if the `right' button is pressed down.
		 * @param up True if the `up' button is pressed down.
		 * @param down True if the `down' button is pressed down.
		 */
		UserInput(bool left, bool right, bool up, bool down, float theta) :
		    _left(left), _right(right), _up(up), _down(down), _theta(theta) { }

		UserInput() :
		    _left(false), _right(false), _up(false), _down(false), _theta(0.0) { }

		UserInput(ReadPacket*);
		void writeToPacket(WritePacket*);

		/**
		 * Returns the acceleration vector indicated by the key input; takes into account
		 * the direction that the player is facing.
		 * @return the acceleration indicated by the key input.
		 */
		Vector2D getAcceleration() const;
};

#endif
