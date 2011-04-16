#include <GL/gl.h>
#include "font.h"
#include <cstdio>
#include <cstdlib>

#define fontmap_width 1024
#define fontmap_height 1024

float character_x1[256];
float character_x2[256];
float character_y1[256];
float character_y2[256];

GLuint font_texture;
unsigned char * bitmapdata;

textquad::textquad(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
	x1 = a;
	y1 = b;
	z1 = c;
	x2 = d;
	y2 = e;
	z2 = f;
	dx = g;
	dy = h;
	dz = i;
}

void textquad::inc(float s) {
	x1 += dx * s;
	x2 += dx * s;
	y1 += dy * s;
	y2 += dy * s;
	z1 += dz * s;
	z2 += dz * s;
}

void init_font() {
	/*for(int i = 0; i < 256; i++) {
		character_y1[i] = 16 * (i / 16);
		character_y2[i] = 16 * (i / 16) + 1;
		for(int j = 16 * (i % 16); j < 16 * (i % 16 + 1); j++) {
			for(int k = character_y1[i]; k < character_y2[i]; k++) {

	}*/
	
	FILE *bitmapfile = fopen("fontmap.bitmap","r");
	bitmapdata = (unsigned char *)malloc(fontmap_width * fontmap_height * 4);
	size_t freadresult = fread(bitmapdata, 1, fontmap_width * fontmap_height * 4, bitmapfile);
	fclose(bitmapfile);

	glGenTextures(1, &font_texture);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, fontmap_width, fontmap_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmapdata);

	free(bitmapdata);

	FILE *fontdata = fopen("font.dat","r");
	freadresult = fread((char *)character_x1, 1, 256*4, fontdata);
	freadresult = fread((char *)character_x2, 1, 256*4, fontdata);
	freadresult = fread((char *)character_y1, 1, 256*4, fontdata);
	freadresult = fread((char *)character_y2, 1, 256*4, fontdata);
	fclose(fontdata);
}

//bool fortesting = true;

void draw_str(textquad tq, char *text) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font_texture);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	for(int i = 0; text[i] != '\0'; i++) {
		float x1 = character_x1[text[i]];
		float x2 = character_x2[text[i]];
		float y1 = character_y1[text[i]];
		float y2 = character_y2[text[i]];
		//printf("%f %f %f %f\n", x1, x2, y1, y2);
		float s = (x2 - x1) * 16.0f;
		glTexCoord2f(x1, y1); glVertex3f(tq.x1, tq.y1, tq.z1);
		glTexCoord2f(x1, y2); glVertex3f(tq.x2, tq.y2, tq.z2);
		glTexCoord2f(x2, y2); glVertex3f(tq.x2 + tq.dx * s, tq.y2 + tq.dy * s, tq.z2 + tq.dz * s);
		glTexCoord2f(x2, y1); glVertex3f(tq.x1 + tq.dx * s, tq.y1 + tq.dy * s, tq.z1 + tq.dz * s);
		tq.inc(s);
		//if(fortesting) {printf("c = %c, s = %f\n", text[i], s);}
		//if(text[i] == ' ')
		//	printf("%f %f %f %f\n", x1, x2, y1, y2);

		/*float x1 = (float)(text[i] % 16) * 0.0625f;
		float y1 = (float)(text[i] / 16) * 0.0625f;
		float x2 = x1 + 0.0625f;
		float y2 = y1 + 0.0625f;
		glTexCoord2f(x1, y1); glVertex3f(tq.x1, tq.y1, tq.z1);
		glTexCoord2f(x1, y2); glVertex3f(tq.x2, tq.y2, tq.z2);
		glTexCoord2f(x2, y2); glVertex3f(tq.x2 + tq.dx, tq.y2 + tq.dy, tq.z2 + tq.dz);
		glTexCoord2f(x2, y1); glVertex3f(tq.x1 + tq.dx, tq.y1 + tq.dy, tq.z1 + tq.dz);
		tq.inc(1.0f);*/

	}
	//fortesting = false;
	//glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
	//glTexCoord2f(0.0f, 0.5f); glVertex3f(0.0f, 0.5f, 0.0f);
	//glTexCoord2f(0.5f, 0.5f); glVertex3f(0.5f, 0.5f, 0.0f);
	//glTexCoord2f(0.5f, 0.0f); glVertex3f(0.5f, 0.0f, 0.0f);
	glDisable(GL_TEXTURE_2D);
	glEnd();
}
