#include "Menu.h"
#include "Font.h"
#include "Render.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

const float kMenuDrawButtonLeft         = 0.05f;
const float kMenuDrawButtonTop          = 0.05f;
const float kMenuDrawButtonWidth        = (1.0f - 2.0f * kMenuDrawButtonLeft);
const float kMenuDrawButtonHeight       = 0.15f;
const float kMenuDrawButtonSeparation   = 0.05f;
const float kMenuDrawFontSize           = 0.05f;
const float kMenuDrawButtonTextPadding  = 0.055f;

const float kMenuDrawToggleDistanceFromBack = 0.2f;

const float kMenuDrawInputLeft              = 0.15f;
const float kMenuDrawInputRight             = 0.85f;
const float kMenuDrawInputTop               = 0.3f;
const float kMenuDrawInputBottom            = 0.7f;
const float kMenuDrawInputTitleLeft         = 0.02f;
const float kMenuDrawInputTitleTop          = 0.02f;
const float kMenuDrawInputTextfieldLeft     = 0.02f;
const float kMenuDrawInputTextfieldTop      = 0.09f;
const float kMenuDrawInputTextfieldWidth    = 0.66f;
const float kMenuDrawInputTextfieldHeight   = 0.07f;

const int8_t kMenuDrawMenuAlpha = 200;

using std::string;

void Menu::drawMenu() {
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

void Menu::draw() {
	if(!is_item_active || menuItems[current_index]->shouldMenuBeDrawn()) {
		glColor4ub(167,155,155,transparent?kMenuDrawMenuAlpha:255);
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glEnd();

		for (int i = 0; i < static_cast<int>(menuItems.size()); i++) {
			menuItems[i]->draw(i == current_index, 
				kMenuDrawButtonLeft,
				kMenuDrawButtonTop + i * (kMenuDrawButtonHeight + kMenuDrawButtonSeparation),
				kMenuDrawButtonWidth,
				kMenuDrawButtonHeight,
				transparent ? kMenuDrawMenuAlpha : 255	
			);
		}
	}

    if (is_item_active) {
		menuItems[current_index]->drawAsActive(transparent?kMenuDrawMenuAlpha:255);
    }
}

#define selected_r 		40
#define selected_g 		78
#define selected_b 		202
#define unselected_r		120
#define unselected_g		140
#define unselected_b		214

static void draw_button(bool selected, string const& text, float x, float y, float width, float height, GLubyte alpha) {
    if (selected) {
		glColor4ub(selected_r,selected_g,selected_b,alpha);
    } else {
		glColor4ub(unselected_r,unselected_g,unselected_b,alpha);
    }

    glBegin(GL_QUADS); {
        glVertex3f(x, y, 0.0f);
        glVertex3f(x, y+height, 0.0f);
        glVertex3f(x+width, y+height, 0.0f);
        glVertex3f(x+width, y, 0.0f);
    } glEnd();

	glColor4ub(255,255,255,kMenuDrawMenuAlpha);
	textquad tq(x + kMenuDrawButtonTextPadding, y + (height - kMenuDrawFontSize) * 0.5f, 0.0f, x + kMenuDrawButtonTextPadding, y + (height + kMenuDrawFontSize) * 0.5f, 0.0f, 0.05f, 0.0f, 0.0f);
	draw_str(tq, text);
}

void MenuItem::drawAsActive(unsigned char) {}

void MenuItem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name_.c_str(), x, y, width, height, alpha);
}

void SubMenuItem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name_.c_str(), x, y, width, height, alpha);
    glBegin(GL_TRIANGLES); {
        glVertex3f(x + 0.01f, y + 0.03f, 0.0f);
        glVertex3f(x + 0.01f, y + 0.12f, 0.0f);
        glVertex3f(x + 0.045f, y + 0.075f, 0.0f);
    } glEnd();
}

void SubMenuItem::drawAsActive(unsigned char) {
	m->draw();
}

void InputMenuItem::drawAsActive(unsigned char alpha) {
    glBegin(GL_QUADS); {
        glColor4ub(75,75,75,alpha);
        glVertex3f(kMenuDrawInputLeft, kMenuDrawInputTop, 0.0f);
        glVertex3f(kMenuDrawInputLeft, kMenuDrawInputBottom, 0.0f);
        glVertex3f(kMenuDrawInputRight, kMenuDrawInputBottom, 0.0f);
        glVertex3f(kMenuDrawInputRight, kMenuDrawInputTop, 0.0f);
        glColor4ub(150,150,150,alpha);
        glVertex3f(kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft, kMenuDrawInputTop + kMenuDrawInputTextfieldTop, 0.0f);
        glVertex3f(kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft + kMenuDrawInputTextfieldWidth, 
                kMenuDrawInputTop + kMenuDrawInputTextfieldTop, 0.0f);
        glVertex3f(kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft + kMenuDrawInputTextfieldWidth,
                kMenuDrawInputTop + kMenuDrawInputTextfieldTop + kMenuDrawInputTextfieldHeight, 0.0f);
        glVertex3f(kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft,
                kMenuDrawInputTop + kMenuDrawInputTextfieldTop + kMenuDrawInputTextfieldHeight, 0.0f);
    } glEnd();

	glColor4ub(0,0,0,255); //Make text opaque

	if ("" != text) {
		textquad tq(kMenuDrawInputLeft + kMenuDrawInputTitleLeft,
				kMenuDrawInputTop + kMenuDrawInputTitleTop,
				0.0f,
				kMenuDrawInputLeft + kMenuDrawInputTitleLeft,
				kMenuDrawInputTop + kMenuDrawInputTitleTop + kMenuDrawFontSize,
				0.0f,
				0.05f, 0.0f, 0.0f);
		draw_str(tq, text);
	}

	if (nullptr != input) {
		textquad tq(kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft + 0.02f,
				kMenuDrawInputTop + kMenuDrawInputTextfieldTop + 0.02f,
				0.0f,
				kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft + 0.02f,
				kMenuDrawInputTop + kMenuDrawInputTextfieldTop + kMenuDrawFontSize + 0.02f,
				0.0f,
				0.05f, 0.0f, 0.0f);
		draw_str(tq, std::string(input, len));
	}

	if (displayError && "" != invalidInputError) {
		glColor4ub(255,0,0,255);
		textquad tq(kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft + 0.02f,
				kMenuDrawInputTop + kMenuDrawInputTextfieldTop + kMenuDrawInputTextfieldHeight + 0.02f,
				0.0f,
				kMenuDrawInputLeft + kMenuDrawInputTextfieldLeft + 0.02f,
				kMenuDrawInputTop + kMenuDrawInputTextfieldTop + kMenuDrawInputTextfieldHeight + kMenuDrawFontSize + 0.02f,
				0.0f,
				0.05f, 0.0f, 0.0f);
		draw_str(tq, invalidInputError);
	}

}	

void ToggleMenuItem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	draw_button(selected, name_, x, y, width, height, alpha);
	textquad tq(x + width - kMenuDrawToggleDistanceFromBack,
			y + (height - kMenuDrawFontSize) * 0.5f,
			0.0f,
			x + width - kMenuDrawToggleDistanceFromBack,
			y + (height + kMenuDrawFontSize) * 0.5f,
			0.0f, 0.05f, 0.0f, 0.0f);
	if(state) {
		glColor4ub(0,255,0,alpha);
		draw_str(tq, "ON");
	} else {
		glColor4ub(255,0,0,alpha);
		draw_str(tq, "OFF");
	}
}

#define slider_left		0.25f
#define slider_right		0.90f
#define slider_slide_width	0.03f
#define slider_slide_height	0.47f
#define slider_tick_top		0.1f
#define slider_tick_bottom	0.5f
#define slider_line_height	0.3f
#define slider_text_top		0.6f
#define slider_text_bottom	0.9f
#define slider_text_name_top	0.1f
#define slider_text_name_bottom	0.4f
#define slider_text_name_left	kMenuDrawButtonLeft + 0.02f

void SliderMenuItem::draw(bool selected, float x, float y, float width, float height, unsigned char alpha) {
	glColor4ub(0,0,0,alpha);
    glBegin(GL_LINES); {
        glVertex3f(x + slider_left  * width, y + slider_line_height * height, 0.0f);
        glVertex3f(x + slider_right * width, y + slider_line_height * height, 0.0f);
        for (int i = 0; i < static_cast<int>(states.size()); i++) {
            float tickx = x + width*(slider_right*(float)i + slider_left*(float)(states.size()-1-i)) / (float)(states.size()-1);
            glVertex3f(tickx, y + slider_tick_top * height, 0.0f);
            glVertex3f(tickx, y + slider_tick_bottom * height, 0.0f);
        }
    } glEnd();

    if (selected) {
		glColor4ub(selected_r,selected_g,selected_b,alpha);
    } else {
		glColor4ub(unselected_r,unselected_g,unselected_b,alpha);
    }
	float rectx1 = x + width * ((slider_right * (float)curstate + slider_left * (float)(states.size()-1-curstate))/(float)(states.size()-1) - 0.5f * slider_slide_width);
	float recty1 = y + height * (slider_line_height - 0.5f*slider_slide_height);
	float recty2 = y + height * (slider_line_height + 0.5f*slider_slide_height);
	glBegin(GL_QUADS);
	glVertex3f(rectx1, recty1, 0.0f);
	glVertex3f(rectx1, recty2, 0.0f);
	glVertex3f(rectx1 + slider_slide_width * width, recty2, 0.0f);
	glVertex3f(rectx1 + slider_slide_width * width, recty1, 0.0f);
	if (curstate != newcurstate) {
		glColor4ub(selected_r,selected_g,selected_b,50);
		rectx1 = x + width * ((slider_right * (float)newcurstate + slider_left * (float)(states.size()-1-newcurstate))/(float)(states.size()-1) - 0.5f * slider_slide_width);
		glVertex3f(rectx1, recty1, 0.0f);
		glVertex3f(rectx1, recty2, 0.0f);
		glVertex3f(rectx1 + slider_slide_width * width, recty2, 0.0f);
		glVertex3f(rectx1 + slider_slide_width * width, recty1, 0.0f);
	}
	glEnd();

	glColor4ub(0,0,0,255);
	for(int i = 0; i < static_cast<int>(states.size()); i++) {
		float tickx = x + width*(slider_right*(float)i + slider_left*(float)(states.size()-1-i)) / (float)(states.size()-1);
		textquad tq(tickx, y+slider_text_top*height, 0.0f,
				tickx, y+slider_text_bottom*height, 0.0f,
				(slider_text_bottom-slider_text_top)*height, 0.0f, 0.0f);
		draw_str_center(tq, states[i]);
	}
	textquad tq(    slider_text_name_left * width, y+slider_text_name_top*height, 0.0f,
			slider_text_name_left * width, y+slider_text_name_bottom*height, 0.0f,
			(slider_text_name_bottom-slider_text_name_top)*height, 0.0f, 0.0f);
	draw_str(tq, name_);
}
