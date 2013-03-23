#include "ClConstants.h"

const sampler_t texSample = CLK_NORMALIZED_COORDS_TRUE|CLK_ADDRESS_REPEAT|CLK_FILTER_LINEAR;

bool isCC(x1, y1, x2, y2, x3, y3) {
	return (x3 - x2) * (y2 - y1) < (y3 - y2) * (x2 - x1);
}

float castRay(
		float x, 
		float y, 
		float z, 
		float xdir, 
		float ydir, 
		float zdir, 
		int obstacles, 
		int roundObstacles,
		__constant float* obspoints, 
		int objects, 
		__constant float* objpoint, 
		__constant float* objsize, 
		int* obstaclep, 
		int* obstacleroundp,
		int* objectp, 
		float when
	) {
	for (int i = 0; i < obstacles; i++) {
		float xdirr = obspoints[4*i+2]-obspoints[4*i], ydirr = obspoints[4*i+3]-obspoints[4*i+1];
		float pos = x*ydirr-y*xdirr, amount = xdir*ydirr-ydir*xdirr;
		float move = (obspoints[4*i]*ydirr-obspoints[4*i+1]*xdirr-pos)/amount;
		float newx = x+move*xdir, newy = y+move*ydir, newz = z+move*zdir;
		if (move > 0 && move*(1-EPS) < when && newz >= 0 && newz < 1) {
			float loc1 = obspoints[4*i]*xdirr+obspoints[4*i+1]*ydirr;
			float loc2 = loc1+xdirr*xdirr+ydirr*ydirr;
			float loc = newx*xdirr+newy*ydirr;
			if (loc >= loc1 && loc <= loc2) {
				when = move*(1-EPS);
				*obstaclep = -1;
				*obstacleroundp = -1;
				*objectp = -1;
			}
		}
	}

	for (int i = 0; i < roundObstacles; i++) {
		float centerx = obspoints[4*obstacles + 5*i];
		float centery = obspoints[4*obstacles + 5*i + 1];
		float radius = obspoints[4*obstacles + 5*i + 2];
		float walltheta1 = obspoints[4*obstacles + 5*i + 3];
		float walltheta2 = obspoints[4*obstacles + 5*i + 4];
		float a = xdir*xdir + ydir*ydir;
		if (fabs(a) > EPS) {
			float b = 2.0 * (xdir * (x - centerx) + ydir * (y - centery));
			float c = (x-centerx)*(x-centerx) + (y-centery)*(y-centery) - radius*radius;
			float d = b*b - 4.0 * a * c;
			if (d >= 0.0) {
				d = sqrt(d);
				float ti = (-b - d) / (2.0 * a);

				if (ti >= 0.0) {
					float newx = x+ti*xdir, newy = y+ti*ydir, newz = z+ti*zdir;
					if (ti > 0 && ti*(1-EPS) < when && newz >= 0 && newz < 1
						&&
						isCC(cos(walltheta1), sin(walltheta1),
							     newx - centerx, newy - centery,
							     cos(walltheta2), sin(walltheta2))) {
						when = ti*(1-EPS);
						*obstaclep = -1;
						*obstacleroundp = -1;
						*objectp = -1;
					}
				}

				/*
				ti = (-b + d) / (2.0 * a);

				if (ti >= 0.0) {
					float newx = x+ti*xdir, newy = y+ti*ydir, newz = z+ti*zdir;
					if (ti > 0 && ti*(1-EPS) < when && newz >= 0 && newz < 1
						&&
						isCC(cos(walltheta1), sin(walltheta1),
							     newx - centerx, newy - centery,
							     cos(walltheta2), sin(walltheta2))) {
						when = ti*(1-EPS);
						*obstaclep = -1;
						*obstacleroundp = -1;
						*objectp = -1;
					}
				}
				*/

			}
		}
	}


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
			*obstacleroundp = -1;
			*objectp = -1;
			when = ans;
		}
	}
	return when;
}

__kernel void 
render(
		float x, 
		float y, 
		float z, 
		float xdir, 
		float ydir, 
		float zdir, 
		int obstacles, 
		int roundObstacles,
		__constant float* obspoints, 
		__constant unsigned char* obscolor, 
		int objects, 
		__constant float* objpoint, 
		__constant float* objsize, 
		__constant unsigned char* objcolor, 
		int lights, 
		__constant float* lightpos, 
		__constant unsigned char* lightcolor, 
		__write_only image2d_t im,
		int WIDTH,
		int HEIGHT,
		float minX,
		float maxX,
		float minY,
		float maxY
	) {
	float nxdir = xdir+((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*ydir);
	float nydir = ydir-((get_global_id(0)-(WIDTH-1)/2.0)/((float)HEIGHT-1)*xdir);
	xdir = nxdir;
	ydir = nydir;
	zdir += (get_global_id(1)-(HEIGHT-1)/2.0)/((float)HEIGHT-1);

	float4 tcolor = (float4)(0, 0, 0, 0);
	float mult = 1;
	for (int s = 0; s < 6; s++) {
		int obstacle = -1;
		int obstacleround = -1;
		int object = -1;
		float when = castRay(x, y, z, xdir, ydir, zdir, obstacles, roundObstacles, obspoints, objects, objpoint, objsize, &obstacle, &obstacleround, &object, 100);

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
		else if (obstacleround != -1) {
			ccolor = (float4)(obscolor[4*(obstacles+obstacleround)]/255.0,
			                  obscolor[4*(obstacles+obstacleround)+1]/255.0,
			                  obscolor[4*(obstacles+obstacleround)+2]/255.0,
			                  obscolor[4*(obstacles+obstacleround)+3]/255.0);
			normal = normalize((float4)(x + xdir*when - obspoints[4*obstacles+5*obstacleround], y + ydir*when - obspoints[4*obstacles+5*obstacleround+1], 0, 0));
			if (dot(dir, normal) > 0) normal = -normal;
			hit = true;
		}
		else if (object != -1) {
			ccolor = (float4)(objcolor[4*object]/255.0, objcolor[4*object+1]/255.0, objcolor[4*object+2]/255.0, objcolor[4*object+3]/255.0);
			normal = normalize((float4)(x+xdir*when-objpoint[2*object], y+ydir*when-objpoint[2*object+1], (z+zdir*when)/objsize[2*object+1]/objsize[2*object+1], 0));
			hit = true;
			specular = true;
		}
		else if (zdir < 0 && x-xdir*z/zdir >= minX && x-xdir*z/zdir <= maxX && y-ydir*z/zdir >= minY && y-ydir*z/zdir <= maxY) {
			float xp = (x-xdir*z/zdir)/3, yp = (y-ydir*z/zdir)/3;
			if (xp < 0) xp += (int)(fabs(xp)/2+1)*2;
			if (yp < 0) yp += (int)(fabs(yp)/2+1)*2;
			ccolor = (((int)xp+(int)yp)%2)*(float4)(1, 1, 1, 0);
			normal = (float4)(0, 0, 1, 0);
			when = -z/zdir;
			hit = true;
			// Floor reflection!!
			specular = true;
		}
		when *= (1-EPS);
		if (hit) {
			color = ccolor*.2;
			for (int i = 0; i < lights; i++) {
				float4 lightdir = (float4)(lightpos[3*i]-x-xdir*when, lightpos[3*i+1]-y-ydir*when, lightpos[3*i+2]-z-zdir*when, 0);
				float w = castRay(x+xdir*when, y+ydir*when, z+zdir*when, lightdir.x, lightdir.y, lightdir.z, obstacles, roundObstacles, obspoints, objects, objpoint, objsize, &obstacle, &obstacleround, &object, 1);
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
