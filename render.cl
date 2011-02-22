#include "constants.h"

#define EPS .000001

const sampler_t texSample = CLK_NORMALIZED_COORDS_TRUE|CLK_ADDRESS_REPEAT|CLK_FILTER_LINEAR;

float castRay(float x, float y, float z, float xdir, float ydir, float zdir, int obstacles, __constant float* obspoints, __constant int* bsp, int objects, __constant float* objpoint, __constant float* objsize, int* obstaclep, int* objectp, float when) {
	int nodeStack[MAX_DEPTH];
	float timeStack[MAX_DEPTH];
	timeStack[0] = when;
	int n = 1;
	int current = 0;
	for (;;) {
		int i = bsp[3*current];
		float xdirr = obspoints[4*i+2]-obspoints[4*i], ydirr = obspoints[4*i+3]-obspoints[4*i+1];
		float pos = -x*ydirr+y*xdirr, amount = -xdir*ydirr+ydir*xdirr;
		float which = -obspoints[4*i]*ydirr+obspoints[4*i+1]*xdirr-pos;
		float move = which/amount;
		float newx = x+move*xdir, newy = y+move*ydir, newz = z+move*zdir;
		if (move > 0 && move*(1-EPS) < timeStack[n-1]) {
			float loc1 = obspoints[4*i]*xdirr+obspoints[4*i+1]*ydirr;
			float loc2 = loc1+xdirr*xdirr+ydirr*ydirr;
			float loc = newx*xdirr+newy*ydirr;
			timeStack[n] = move*(1-EPS);
			if (loc >= loc1 && loc <= loc2 && newz >= 0 && newz < 1) nodeStack[n] = -i-1;
			else if (which < 0 && bsp[3*current+1] != -1) nodeStack[n] = bsp[3*current+1];
			else if (bsp[3*current+2] != -1) nodeStack[n] = bsp[3*current+2];
			else n--;
			n++;
		}
		
		if (which < 0) current = bsp[3*current+2];
		else current = bsp[3*current+1];
		if (current == -1) {
			n--;
			if (n == 0 || nodeStack[n] < 0) break;
			current = nodeStack[n];
		}
	}
	when = timeStack[n];
	if (n > 0) *obstaclep = -nodeStack[n]-1;
	
	for (int i = 0; i < objects; i++) {
		float xpos = (x-objpoint[2*i])/objsize[2*i];
		float ypos = (y-objpoint[2*i+1])/objsize[2*i];
		float zpos = z/objsize[2*i]/objsize[2*i+1];
		float xve = xdir/objsize[2*i];
		float yve = ydir/objsize[2*i];
		float zve = zdir/objsize[2*i]/objsize[2*i+1];
		float a = xve*xve+yve*yve+zve*zve;
		float b = 2*(xpos*xve+ypos*yve+zpos*zve);
		if (b >= 0) continue;
		float c = xpos*xpos+ypos*ypos+zpos*zpos-1;
		float discrim = b*b-4*a*c;
		if (discrim <= 0) continue;
		float ans = (-b-sqrt(discrim))/(2*a);
		if (ans < when && z+zdir*ans > 0) {
			*obstaclep = -1;
			*objectp = i;
			when = ans;
		}
	}
	return when;
}

__kernel void render(float x, float y, float z, float xdir, float ydir, float zdir, int obstacles, __constant float* obspoints, __constant unsigned char* obscolor, __constant int* bsp, int objects, __constant float* objpoint, __constant float* objsize, __constant unsigned char* objcolor, int lights, __constant float* lightpos, __constant unsigned char* lightcolor, __write_only image2d_t im, __read_only image2d_t grass) {
	float nxdir = xdir+((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*ydir);
	float nydir = ydir-((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*xdir);
	xdir = nxdir;
	ydir = nydir;
	zdir += (get_global_id(1)-(HEIGHT-1)/2.0)/((float)HEIGHT-1);
	
	float4 tcolor = (float4)(0, 0, 0, 0), mult = 1;
	for (int s = 0; s < 6; s++) {
		int obstacle = -1;
		int object = -1;
		float when = 100;
		if (zdir < 0) when = -z/zdir;
		when = castRay(x, y, z, xdir, ydir, zdir, obstacles, obspoints, bsp, objects, objpoint, objsize, &obstacle, &object, when);
		
		float4 color = (float4)(0, .5, 1, 1);
		float4 ccolor;
		float4 normal;
		float4 dir = normalize((float4)(xdir, ydir, zdir, 0));
		bool hit = false;
		bool specular = false;
	
		if (obstacle != -1) {
			ccolor = (float4)(obscolor[4*obstacle]/255.0, obscolor[4*obstacle+1]/255.0, obscolor[4*obstacle+2]/255.0, obscolor[4*obstacle+3]/255.0);
			normal = normalize((float4)(obspoints[4*obstacle+1]-obspoints[4*obstacle+3], obspoints[4*obstacle+2]-obspoints[4*obstacle], 0, 0));
			if (dot(dir, normal) > 0) normal = -normal;
			hit = true;
		}
		else if (object != -1) {
			ccolor = (float4)(objcolor[4*object]/255.0, objcolor[4*object+1]/255.0, objcolor[4*object+2]/255.0, objcolor[4*object+3]/255.0);
			normal = normalize((float4)(x+xdir*when-objpoint[2*object], y+ydir*when-objpoint[2*object+1], (z+zdir*when)/objsize[2*object+1]/objsize[2*object+1], 0));
			hit = true;
			specular = true;
		}
		else if (zdir < 0 && x-xdir*z/zdir >= MIN_X && x-xdir*z/zdir <= MAX_X && y-ydir*z/zdir >= MIN_Y && y-ydir*z/zdir <= MAX_Y) {
			ccolor = read_imagef(grass, texSample, (float2)(x-xdir*z/zdir, y-ydir*z/zdir)/100.0);
			normal = (float4)(0, 0, 1, 0);
			when = -z/zdir;
			hit = true;
		}
		when *= (1-EPS);
		if (hit) {
			color = ccolor*.2;
			for (int i = 0; i < lights; i++) {
				float4 lightdir = (float4)(lightpos[3*i]-x-xdir*when, lightpos[3*i+1]-y-ydir*when, lightpos[3*i+2]-z-zdir*when, 0);
				float w = castRay(x+xdir*when, y+ydir*when, z+zdir*when, lightdir.x, lightdir.y, lightdir.z, obstacles, obspoints, bsp, objects, objpoint, objsize, &obstacle, &object, 1);
				if (w != 1) continue;
			
				lightdir = normalize(lightdir);
				if (dot(lightdir, normal) <= 0) continue;
				color += .6*dot(lightdir, normal)*ccolor*(float4)(lightcolor[4*i]/255.0, lightcolor[4*i+1]/255.0, lightcolor[4*i+2]/255.0, lightcolor[4*i+3]/255.0);
			}
		}
		
		for (int i = 0; i < lights; i++) {
			float xdiff = lightpos[3*i]-x, ydiff = lightpos[3*i+1]-y, zdiff = lightpos[3*i+2]-z;
			float4 diffv = (float4)(xdiff, ydiff, zdiff, 0);
			diffv -= dot(diffv, dir)*dir;
			float smallest = dot(diffv, diffv);
			if (xdiff*xdir+ydiff*ydir+zdiff*zdir <= 0) smallest = xdiff*xdiff+ydiff*ydiff+zdiff*zdiff;
			if (xdiff*xdir+ydiff*ydir+zdiff*zdir >= when*(xdir*xdir+ydir*ydir+zdir*zdir)) smallest = (xdiff-when*xdir)*(xdiff-when*xdir)+(ydiff-when*ydir)*(ydiff-when*ydir)+(zdiff-when*zdir)*(zdiff-when*zdir);
			color += (float4)(lightcolor[4*i]/255.0, lightcolor[4*i+1]/255.0, lightcolor[4*i+2]/255.0, lightcolor[4*i+3]/255.0)*exp(-smallest/8.0)*3;
		}
		
		tcolor += color*mult;
		mult *= .2;
		if (!specular) break;
		x += xdir*when;
		y += ydir*when;
		z += zdir*when;
		float4 nextdir = dir-2*dot(normal, dir)*normal;
		xdir = nextdir.x;
		ydir = nextdir.y;
		zdir = nextdir.z;
	}
	
	write_imagef(im, (int2)(get_global_id(0), get_global_id(1)), tcolor);
}
