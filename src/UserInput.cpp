#include "UserInput.h"
#include "Constants.h"

UserInput::UserInput(ReadPacket *rp) {
    const uint8_t mask = rp->read_char();
    _left = (mask & 1);
    _right = ((mask >> 1) & 1);
    _up = ((mask >> 2) & 1);
    _down = ((mask >> 3) & 1);
    _theta = rp->read_float();
}

void UserInput::writeToPacket(WritePacket *wp) const {
    const uint8_t mask =
        static_cast<uint8_t>(_left | (_right << 1) | (_up << 2) | (_down << 3));
    wp->write_char(mask);
    wp->write_float(_theta);
}

Vector2D UserInput::getAcceleration() const {
    Vector2D acceleration = Vector2D(0.0f, 0.0f);
    int value = 0;

    if (_up) {
        acceleration += Vector2D(0.0f, -1.0f);
        value += 1;
    }
    if (_down) {
        acceleration += Vector2D(0.0f, 1.0f);
        value -= 1;
    }
    if (_left) {
        acceleration += Vector2D(1.0f, 0.0f);
        value += 2;
    }
    if (_right) {
        acceleration += Vector2D(-1.0f, 0.0f);
        value -= 2;
    }

    // value is nonzero if and only if acceleration should be nonzero
    if (value != 0) {
        return acceleration.getNormalVector().getRotation(_theta) *
               KEYPRESS_ACCELERATION;
    } else {
        return Vector2D(0.0f, 0.0f);
    }
}
