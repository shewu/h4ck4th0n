#include "Material.h"

virtual void Color::writeToPacket(WritePacket *wp) {
	wp->write_char(r);
	wp->write_char(g);
	wp->write_char(b);
	wp->write_char(a);
}

Material *Material::readFromPacket(ReadPacket *rp) {
	unsigned char r = (unsigned char) rp->read_char();
	unsigned char g = (unsigned char) rp->read_char();
	unsigned char b = (unsigned char) rp->read_char();
	unsigned char a = (unsigned char) rp->read_char();
	return new Color(r,g,b,a);
}
