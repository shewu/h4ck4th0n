#include "Material.h"

Material *Material::readFromPacket(ReadPacket *rp) {
    unsigned char r = (unsigned char)rp->read_char();
    unsigned char g = (unsigned char)rp->read_char();
    unsigned char b = (unsigned char)rp->read_char();
    unsigned char a = (unsigned char)rp->read_char();
    return new Color(r, g, b, a);
}
