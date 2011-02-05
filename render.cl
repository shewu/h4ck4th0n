#include "constants.h"

__kernel void render(float x, float y, float z, float xdir, float ydir, float zdir, int obstacles, __constant float* obspoints, __constant unsigned char* obscolor, int objects, __constant float* objpoint, __constant float* objsize, __constant unsigned char* objcolor, __write_only image2d_t im) {
	int obstacle = -1;
	int object = -1;
	float nxdir = xdir+((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*ydir);
	float nydir = ydir-((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*xdir);
	xdir = nxdir;
	ydir = nydir;
	zdir += (get_global_id(1)-(HEIGHT-1)/2.0)/((float)HEIGHT-1);
	float when = 100;
	for (int i = 0; i < obstacles; i++) {
		float xdirr = obspoints[4*i+2]-obspoints[4*i], ydirr = obspoints[4*i+3]-obspoints[4*i+1];
		float pos = x*ydirr-y*xdirr, amount = xdir*ydirr-ydir*xdirr;
		float move = (obspoints[4*i]*ydirr-obspoints[4*i+1]*xdirr-pos)/amount;
		float newx = x+move*xdir, newy = y+move*ydir, newz = z+move*zdir;
		if (move > 0 && move < when && newz >= 0 && newz < 1) {
			float loc1 = obspoints[4*i]*xdirr+obspoints[4*i+1]*ydirr;
			float loc2 = loc1+xdirr*xdirr+ydirr*ydirr;
			float loc = newx*xdirr+newy*ydirr;
			if (loc >= loc1 && loc <= loc2) {
				when = move;
				obstacle = i;
				object = -1;
			}
		}
	}
	for (int i = 0; i < objects; i++) {
		float xpos = (x-objpoint[2*i])/objsize[2*i];
		float ypos = (y-objpoint[2*i+1])/objsize[2*i];
		float zpos = z/objsize[2*i+1];
		float xve = xdir/objsize[2*i];
		float yve = ydir/objsize[2*i];
		float zve = zdir/objsize[2*i+1];
		float a = xve*xve+yve*yve+zve*zve;
		float b = 2*(xpos*xve+ypos*yve+zpos*zve);
		if (b >= 0) continue;
		float c = xpos*xpos+ypos*ypos+zpos*zpos-1;
		float discrim = b*b-4*a*c;
		if (discrim <= 0) continue;
		float ans = (-b-sqrt(discrim))/(2*a);
		if (ans > 0 && ans < when && z+zdir*ans > 0) {
			obstacle = -1;
			object = i;
			when = ans;
		}
	}
	float4 color = (float4)(0, .5, 1, 1);
	if (zdir < 0 && x-xdir*z/zdir >= MIN_X && x-xdir*z/zdir <= MAX_X && y-ydir*z/zdir >= MIN_Y && y-ydir*z/zdir <= MAX_Y) color = (float4)(0, 1, 0, 1);
	if (obstacle != -1) color = (float4)(obscolor[4*obstacle]/255.0, obscolor[4*obstacle+1]/255.0, obscolor[4*obstacle+2]/255.0, obscolor[4*obstacle+3]/255.0);
	if (object != -1) color = (float4)(objcolor[4*object]/255.0, objcolor[4*object+1]/255.0, objcolor[4*object+2]/255.0, objcolor[4*object+3]/255.0);
	write_imagef(im, (int2)(get_global_id(0), get_global_id(1)), color);
}
