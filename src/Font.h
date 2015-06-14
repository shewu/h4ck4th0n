#ifndef FONT_H
#define FONT_H

#include <string>

struct textquad {
	float x1, y1, z1, x2, y2, z2, dx, dy, dz;
	textquad(float,float,float,float,float,float,float,float,float);
	void inc(float s);

};

void draw_str(textquad tq, std::string const& text);
void draw_str_center(textquad tq, std::string const& text);

void init_font();

#endif
