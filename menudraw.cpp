#include "menu.h"
#include "font.h"
#include "render.h"
#include <GL/gl.h>

#define button_left		0.05f
#define button_top		0.05f
#define button_width		(1.0f - 2.0f * button_left) //0.75f
#define button_height		0.15f
#define button_separation	0.05f
#define font_size		0.05f
#define button_text_padding	0.055f

#define toggle_dist_from_back	0.2f

#define input_left		0.15f
#define input_right		0.85f
#define input_top		0.3f
#define input_bottom		0.7f
#define input_title_left	0.02f
#define input_title_top		0.02f
#define input_textfield_left	0.02f
#define input_textfield_top	0.09f
#define input_textfield_width	0.66f
#define input_textfield_height	0.07f

#define menu_alpha 200

void menu::drawMenu() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glOrtho((-x1-WIDTH/float(HEIGHT)/2.0f+1/2.0f)/(x2-x1),(1.0f-x1+WIDTH/float(HEIGHT)/2.0f-1/2.0f)/(x2-x1),(1.0f-y1)/(y2-y1),-y1/(y2-y1),-1.0f,1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	draw();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void menu::draw() {
	if(!is_item_active || menuitems[current_index]->shouldMenuBeDrawn()) {
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
}

#define selected_r 		40
#define selected_g 		78
#define selected_b 		202
#define unselected_r		120
#define unselected_g		140
#define unselected_b		214

void draw_button(bool selected, char *text, float x, float y, float width, float height, GLubyte alpha) {
	if(selected)
		glColor4ub(selected_r,selected_g,selected_b,alpha);
	else
		glColor4ub(unselected_r,unselected_g,unselected_b,alpha);

	glBegin(GL_QUADS);
	glVertex3f(x, y, 0.0f);
	glVertex3f(x, y+height, 0.0f);
	glVertex3f(x+width, y+height, 0.0f);
	glVertex3f(x+width, y, 0.0f);
	glEnd();

	glColor4ub(255,255,255,menu_alpha);
	textquad tq(x + button_text_padding, y + (height - font_size) * 0.5f, 0.0f, x + button_text_padding, y + (height + font_size) * 0.5f, 0.0f, 0.05f, 0.0f, 0.0f);
	draw_str(tq, text);
}

void menuitem::drawAsActive(unsigned char alpha) {}

void menuitem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name, x, y, width, height, alpha);
}

void submenuitem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name, x, y, width, height, alpha);
	glBegin(GL_TRIANGLES);
	glVertex3f(x + 0.01f, y + 0.03f, 0.0f);
	glVertex3f(x + 0.01f, y + 0.12f, 0.0f);
	glVertex3f(x + 0.045f, y + 0.075f, 0.0f);
	glEnd();
}

void submenuitem::drawAsActive(unsigned char alpha) {
	m->draw();
}

void inputmenuitem::drawAsActive(unsigned char alpha) {
	glBegin(GL_QUADS);
	glColor4ub(75,75,75,alpha);
	glVertex3f(input_left, input_top, 0.0f);
	glVertex3f(input_left, input_bottom, 0.0f);
	glVertex3f(input_right, input_bottom, 0.0f);
	glVertex3f(input_right, input_top, 0.0f);
	glColor4ub(150,150,150,alpha);
	glVertex3f(input_left + input_textfield_left, input_top + input_textfield_top, 0.0f);
	glVertex3f(input_left + input_textfield_left + input_textfield_width, 
			input_top + input_textfield_top, 0.0f);
	glVertex3f(input_left + input_textfield_left + input_textfield_width,
			input_top + input_textfield_top + input_textfield_height, 0.0f);
	glVertex3f(input_left + input_textfield_left,
			input_top + input_textfield_top + input_textfield_height, 0.0f);
	glEnd();

	glColor4ub(0,0,0,255); //Make text opaque

	if(text != NULL) {
		textquad tq(input_left + input_title_left,
				input_top + input_title_top,
				0.0f,
				input_left + input_title_left,
				input_top + input_title_top + font_size,
				0.0f,
				0.05f, 0.0f, 0.0f);
		draw_str(tq, text);
	}

	if(input != NULL) {
		textquad tq(input_left + input_textfield_left + 0.02f,
				input_top + input_textfield_top + 0.02f,
				0.0f,
				input_left + input_textfield_left + 0.02f,
				input_top + input_textfield_top + font_size + 0.02f,
				0.0f,
				0.05f, 0.0f, 0.0f);
		draw_str(tq, input);
	}

	if(displayError && invalidInputError != NULL) {
		glColor4ub(255,0,0,255);
		textquad tq(input_left + input_textfield_left + 0.02f,
				input_top + input_textfield_top + input_textfield_height + 0.02f,
				0.0f,
				input_left + input_textfield_left + 0.02f,
				input_top + input_textfield_top + input_textfield_height + font_size + 0.02f,
				0.0f,
				0.05f, 0.0f, 0.0f);
		draw_str(tq, invalidInputError);
	}

}	

void togglemenuitem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name, x, y, width, height, alpha);
	textquad tq(x + width - toggle_dist_from_back,
			y + (height - font_size) * 0.5f,
			0.0f,
			x + width - toggle_dist_from_back,
			y + (height + font_size) * 0.5f,
			0.0f, 0.05f, 0.0f, 0.0f);
	if(state) {
		glColor4ub(0,255,0,alpha);
		draw_str(tq, (char*)"ON");
	} else {
		glColor4ub(255,0,0,alpha);
		draw_str(tq, (char*)"OFF");
	}
}

#define slider_left		0.15f
#define slider_right		0.90f
#define slider_slide_width	0.03f
#define slider_slide_height	0.47f
#define slider_tick_top		0.1f
#define slider_tick_bottom	0.5f
#define slider_line_height	0.3f

void slidermenuitem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	glColor4ub(0,0,0,alpha);
	glBegin(GL_LINES);
	glVertex3f(x + slider_left  * width, y + slider_line_height * height, 0.0f);
	glVertex3f(x + slider_right * width, y + slider_line_height * height, 0.0f);
	for(int i = 0; i < len; i++) {
		float tickx = x + width*(slider_right*(float)i + slider_left*(float)(len-1-i)) / (float)(len-1);
		glVertex3f(tickx, y + slider_tick_top * height, 0.0f);
		glVertex3f(tickx, y + slider_tick_bottom * height, 0.0f);
	}
	glEnd();

	if(selected)
		glColor4ub(selected_r,selected_g,selected_b,alpha);
	else
		glColor4ub(unselected_r,unselected_g,unselected_b,alpha);
	float rectx1 = x + width * ((slider_right * (float)curstate + slider_left * (float)(len-1-curstate))/(float)(len-1) - 0.5f * slider_slide_width);
	float recty1 = y + height * (slider_line_height - 0.5f*slider_slide_height);
	float recty2 = y + height * (slider_line_height + 0.5f*slider_slide_height);
	glBegin(GL_QUADS);
	glVertex3f(rectx1, recty1, 0.0f);
	glVertex3f(rectx1, recty2, 0.0f);
	glVertex3f(rectx1 + slider_slide_width * width, recty2, 0.0f);
	glVertex3f(rectx1 + slider_slide_width * width, recty1, 0.0f);
	glEnd();
}
