#ifndef SOUND_H
#define SOUND_H

#include <string>

#include "Vector.h"
#include "Packet.h"

enum SoundType {
    SOUND_DING,
    SOUND_BOING,
    SOUND_BOING2,
    SOUND_SPLAT,
    SOUND_SPLAT2,

    NUM_SOUND_TYPES
};

static const std::string kSoundFilenames[] = {"ding", "boing", "boing2",
                                              "splat", "splat2"};

struct Sound {
    SoundType type;
    Vector2D pos;

    Sound() : type(SOUND_DING), pos(0.0f, 0.0f) {}
    Sound(SoundType type, Vector2D const &pos) : type(type), pos(pos) {}
    explicit Sound(ReadPacket *rp) {
        type = (SoundType)rp->read_char();
        pos.x = rp->read_float();
        pos.y = rp->read_float();
    }

    void writeToPacket(WritePacket *wp) const {
        wp->write_char((char)type);
        wp->write_float(pos.x);
        wp->write_float(pos.y);
    }
};

#endif
