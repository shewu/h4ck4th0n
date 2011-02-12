#include "constants.h"

#define EPS .0001

const sampler_t texSample = CLK_NORMALIZED_COORDS_TRUE|CLK_ADDRESS_REPEAT|CLK_FILTER_LINEAR;

float castRay(float x, float y, float z, float xdir, float ydir, float zdir, int obstacles, __constant float* obspoints, int objects, __constant float* objpoint, __constant float* objsize, int* obstaclep, int* objectp, float when) {
	for (int i = 0; i < obstacles; i++) {
		float xdirr = obspoints[4*i+2]-obspoints[4*i], ydirr = obspoints[4*i+3]-obspoints[4*i+1];
		float pos = x*ydirr-y*xdirr, amount = xdir*ydirr-ydir*xdirr;
		float move = (obspoints[4*i]*ydirr-obspoints[4*i+1]*xdirr-pos)/amount;
		float newx = x+move*xdir, newy = y+move*ydir, newz = z+move*zdir;
		if (move > EPS && move < when && newz >= 0 && newz < 1) {
			float loc1 = obspoints[4*i]*xdirr+obspoints[4*i+1]*ydirr;
			float loc2 = loc1+xdirr*xdirr+ydirr*ydirr;
			float loc = newx*xdirr+newy*ydirr;
			if (loc >= loc1 && loc <= loc2) {
				when = move;
				*obstaclep = i;
				*objectp = -1;
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
		if (ans > -EPS && ans < when && z+zdir*ans > 0) {
			*obstaclep = -1;
			*objectp = i;
			when = ans;
		}
	}
	return when;
}

__kernel void render(float x, float y, float z, float xdir, float ydir, float zdir, int obstacles, __constant float* obspoints, __constant unsigned char* obscolor, int objects, __constant float* objpoint, __constant float* objsize, __constant unsigned char* objcolor, int lights, __constant float* lightpos, __constant unsigned char* lightcolor, __write_only image2d_t im, __read_only image2d_t grass) {
	float nxdir = xdir+((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*ydir);
	float nydir = ydir-((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*xdir);
	xdir = nxdir;
	ydir = nydir;
	zdir += (get_global_id(1)-(HEIGHT-1)/2.0)/((float)HEIGHT-1);
	
	float4 tcolor = (float4)(0, 0, 0, 0), mult = 1;
	for (int s = 0; s < 5; s++) {
		int obstacle = -1;
		int object = -1;
		float when = castRay(x, y, z, xdir, ydir, zdir, obstacles, obspoints, objects, objpoint, objsize, &obstacle, &object, 100);
		
		float4 color = (float4)(0, .5, 1, 1);
		float4 ccolor;
		float3 normal;
		float3 dir = normalize((float3)(xdir, ydir, zdir));
		bool hit = false;
		bool specular = false;
	
		if (obstacle != -1) {
			ccolor = (float4)(obscolor[4*obstacle]/255.0, obscolor[4*obstacle+1]/255.0, obscolor[4*obstacle+2]/255.0, obscolor[4*obstacle+3]/255.0);
			normal = normalize((float3)(obspoints[4*obstacle+1]-obspoints[4*obstacle+3], obspoints[4*obstacle+2]-obspoints[4*obstacle], 0));
			if (dot(dir, normal) > 0) normal = -normal;
			hit = true;
		}
		else if (object != -1) {
			ccolor = (float4)(objcolor[4*object]/255.0, objcolor[4*object+1]/255.0, objcolor[4*object+2]/255.0, objcolor[4*object+3]/255.0);
			normal = normalize((float3)(x+xdir*when-objpoint[2*object], y+ydir*when-objpoint[2*object+1], (z+zdir*when)*objsize[2*object]*objsize[2*object]/objsize[2*object+1]/objsize[2*object+1]));
			hit = true;
			specular = true;
		}
		else if (zdir < 0 && x-xdir*z/zdir >= MIN_X && x-xdir*z/zdir <= MAX_X && y-ydir*z/zdir >= MIN_Y && y-ydir*z/zdir <= MAX_Y) {
			ccolor = read_imagef(grass, texSample, (float2)(x-xdir*z/zdir, y-ydir*z/zdir)/20.0);
			normal = (float3)(0, 0, 1);
			when = -z/zdir;
			hit = true;
		}
		if (hit) {
			color = ccolor*.1;
			for (int i = 0; i < lights; i++) {
				float3 lightdir = (float3)(lightpos[3*i]-x-xdir*when, lightpos[3*i+1]-y-ydir*when, lightpos[3*i+2]-z-zdir*when);
				float w = castRay(x+xdir*when, y+ydir*when, z+zdir*when, lightdir.x, lightdir.y, lightdir.z, obstacles, obspoints, objects, objpoint, objsize, &obstacle, &object, 1);
				if (w != 1) continue;
			
				lightdir = normalize(lightdir);
				if (dot(lightdir, normal) <= 0) continue;
				color += .4*dot(lightdir, normal)*ccolor*(float4)(lightcolor[4*i]/255.0, lightcolor[4*i+1]/255.0, lightcolor[4*i+2]/255.0, lightcolor[4*i+3]/255.0);
			}
		}
		tcolor += color*mult;
		mult *= .1;
		if (!specular) break;
		x += xdir*when;
		y += ydir*when;
		z += zdir*when;
		float3 nextdir = dir-2*dot(normal, dir)*normal;
		xdir = nextdir.x;
		ydir = nextdir.y;
		zdir = nextdir.z;
	}
	
	write_imagef(im, (int2)(get_global_id(0), get_global_id(1)), tcolor);
}
