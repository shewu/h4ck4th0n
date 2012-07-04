#include "UserInput.h"
#include "Constants.h"

Vector2D UserInput::getAcceleration() const {
	Vector2D acceleration = Vector2D(0.0f, 0.0f);
	int value = 0;

	if(_up) {
		acceleration += Vector2D(0.0, 1.0);
		value += 1;
	}
	if(_down) {
		acceleration += Vector2D(0.0, -1.0);
		value -= 1;
	}
	if(_left) {
		acceleration += Vector2D(-1.0, 0.0);
		value += 2;
	}
	if(_right) {
		acceleration += Vector2D(1.0, 0.0);
		value -= 2;
	}

	// value is nonzero if and only if acceleration should be nonzero
	if(value != 0) {
		return acceleration.getNormalVector().getRotation(_theta) * KEYPRESS_ACCELERATION;
	} else {
		return Vector2D(0.0, 0.0);
	}
}
