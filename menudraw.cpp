#include "menu.h"
#include "font.h"
#include <GL/gl.h>

#define button_left		0.05f
#define button_top		0.05f
#define button_width		1.0f - 2.0f * button_left //0.75f
#define button_height		0.15f
#define button_separation	0.05f
#define font_size		0.05f

#define input_left		0.15f
#define input_right		0.85f
#define input_top		0.3f
#define input_bottom		0.7f

#define menu_alpha 200

void menu::draw() {
	if(!is_item_active || menuitems[current_index]->shouldMenuBeDrawn()) {

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		//glOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);
		glOrtho(-x1/(x2-x1),(1.0f-x2)/(x2-x1)+1.0f,(1.0f-y2)/(y2-y1)+1.0f,-y1/(y2-y1),-1.0f,1.0f);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4ub(167,155,155,transparent?menu_alpha:255);
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();
	
		for(int i = 0; i < menuitems.size(); i++) {
			menuitems[i]->draw(i == current_index, 
				button_left,
				button_top + i * (button_height + button_separation),
				button_width,
				button_height,
				transparent ? menu_alpha : 255	
			);
		}
	}

	if(is_item_active)
		menuitems[current_index]->drawAsActive(transparent?menu_alpha:255);

	if(!is_item_active || menuitems[current_index]->shouldMenuBeDrawn()) {
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
}

void draw_button(bool selected, char *text, float x, float y, float width, float height, GLubyte alpha) {
	if(selected)
		glColor4ub(40,78,202,alpha);
	else
		glColor4ub(120,140,214,alpha);
	glBegin(GL_QUADS);
	glVertex3f(x, y, 0.0f);
	glVertex3f(x, y+height, 0.0f);
	glVertex3f(x+width, y+height, 0.0f);
	glVertex3f(x+width, y, 0.0f);
	glEnd();

	glColor4ub(255,255,255,menu_alpha);
	textquad tq(x + 0.025f, y + (height - font_size) * 0.5f, 0.0f, x + 0.025f, y + (height + font_size) * 0.5f, 0.0f, 0.05f, 0.0f, 0.0f);
	draw_str(tq, text);

	glDisable(GL_TEXTURE_2D);
}

void menuitem::drawAsActive(unsigned char alpha) {}

void menuitem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name, x, y, width, height, alpha);
}

void submenuitem::drawAsActive(unsigned char alpha) {
	m->draw();
}

void inputmenuitem::drawAsActive(unsigned char alpha) {
	glColor4ub(75,75,75,alpha);
	glBegin(GL_QUADS);
	glVertex3f(input_left, input_top, 0.0f);
	glVertex3f(input_left, input_bottom, 0.0f);
	glVertex3f(input_right, input_bottom, 0.0f);
	glVertex3f(input_right, input_top, 0.0f);
	glEnd();
}	
