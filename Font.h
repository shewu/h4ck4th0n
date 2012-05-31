#ifndef __FONT_H__
#define __FONT_H__

struct textquad {
	float x1, y1, z1, x2, y2, z2, dx, dy, dz;
	textquad(float,float,float,float,float,float,float,float,float);
	void inc(float s);
	
};

void draw_str(textquad tq, char *text);
void draw_str_center(textquad tq, char *text);

void init_font();

#endif

