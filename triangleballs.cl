#define EPSILON 0.000001
#define BOUNCES 4
#define BGCOLOR (0.0f, 0.0f, 0.0f, 0.0f);
#include "common.h"

typedef struct {
	float4 diffuse, specular;
} material;

typedef struct {
	float4 *v0, *v1, *v2, normal;
	material *m;
} triangle;

typedef struct {
	float d;
	float4 normal;
	material *m;
} plane;

typedef struct {
	float2 p1, p2;
	float2 normal;
	float height;
	material *m;
} wall;

typedef struct {
	float4 center;
	float radius;
	material *m;
} sphere;

typedef struct {
	float4 pos, color;
} light;

typedef struct {
	float4 origin, dir;
} ray;

typedef struct {
	float t;
	float4 normal;
	material *m;
} hit;

typedef struct {
	sphere* sphere;
	int num_spheres;
	plane* plane;
	int num_planes;
	wall* wall;
	int num_walls;
	triangle* triangle;
	int num_triangles;
	light* light;
	int num_lights;
	float4 ambient;
} scene;

int intersect_triangle(ray* r, triangle* t, hit* h) {
	float4 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det, u, v;
	
	edge1 = *(t->v1) - *(t->v0);
	edge2 = *(t->v2) - *(t->v0);
	pvec = cross(r->dir, edge2);
	det = dot(edge1, pvec);

	if (det > -EPSILON && det < EPSILON) return 0;
	inv_det = 1.0f / det;
	tvec = ray->origin - *(t->v0);
	
	u = dot(tvec, pvec) * inv_det;
	if (u < 0.0f || u > 1.0f) return 0;
	
	qvec = cross(tvec, edge1);
	
	v = dot(ray->dir, qvec) * inv_det;
	if (v < 0.0f || u + v > 1.0f) return 0;
	
	h->t = dot(edge2, qvec) * inv_det;
	if (h->t < EPSILON) return 0;
	h->normal = t->normal;
	h->m = t->m;
	return 1;	
}

int intersect_sphere(ray* r, sphere* s, hit* h)
{
	float a = dot(r->dir, r->dir);
	float b = 2 * dot(r->origin - s->center, r->dir);
	float c = dot(r->origin - s->center, r->origin - s->center) - s->radius * s->radius;
	float disc = b * b - 4 * a * c;
	if (disc < 0) return 0;
	float q = -0.5 * (b + sign(b) * sqrt(disc));
	float t1 = q / a;
	if (t1 > EPSILON) {
		h->t = t1;
		h->normal = r->origin + t1 * r->dir - s->center;
		h->m = s->m;
		return 1;
	}
	float t2 = c / q;
	if (t2 > EPSILON) {
		h->t = t2;
		h->normal = fast_normalize(r->origin + t1 * r->dir - s->center);
		h->m = s->m;
		return 1;
	}
	return 0;
}

int intersect_plane(ray* r, plane* p, hit* h)
{
	float nd = dot(p->normal, ray->dir);
	if (nd == 0.0f) return 0;
	float t = (p->d - dot(p->normal, ray->origin)) / nd;
	if (t < EPSILON) return 0;
	h->t = t;
	h->normal = p->normal;
	h->m = p->m;	
}

int intersect_wall(ray* r, wall* w, hit* h)
{
	float2 of2 = convert_float2(r->origin);
	float2 df2 = convert_float2(r->dir);
	float nd = dot(w->normal, df2);
	if (nd == 0.0f) return 0;
	float t = dot(w->p1 - of2, w->normal) / nd;
	if (t < EPSILON) return 0;
	float4 p = r->origin + t * r->dir;
	if (p.z < 0.0f || p.z > w->height) return 0;
	float2 wdir = w->p2 - w->p1;
	if (wdir.x != 0.0f) {
		float wt = (p.x - w->p1.x) / wdir.x;
		if (wt < 0.0f || wt > 1.0f) return 0;
		h->t = t;
		h->normal = w->normal;
		h->m = w->m;
		return 1;
	}	
	float wt = (p.y - w->p1.y) /wdir.y;
	if (wt < 0.0f || wt > 1.0f) return 0;
	h->t = t;
	h->normal = w->normal;
	h->m = w->m;
	return 1;
}

int intersect_scene(ray* r, scene* s, hit* h)
{
	hit temp_h;
	int result = 0;
	h->t = MAXFLOAT;
	for (int i = 0; i < s->num_spheres; i++) {
		if (intersect_sphere(r, &(s->sphere[i]), &temp_h)) {
			if (temp_h.t < h->t ) {
				h->t = temp_h.t;
				h->m = temp_h.m;
				h->normal = temp_h.normal;
			}
			result = 1;
		}
	}
	for (int i = 0; i < s->num_planes; i++) {
		if (intersect_plane(r, &(s->plane[i]), &temp_h)) {
			if (temp_h.t < h->t ) {
				h->t = temp_h.t;
				h->m = temp_h.m;
				h->normal = temp_h.normal;
			}
			result = 1;
		}
	}
	for (int i = 0; i < s->num_walls; i++) {
		if (intersect_wall;;(r, &(s->wall[i]), &temp_h)) {
			if (temp_h.t < h->t ) {
				h->t = temp_h.t;
				h->m = temp_h.m;
				h->normal = temp_h.normal;
			}
			result = 1;
		}
	}
	for (int i = 0; i < s->num_triangles; i++) {
		if (intersect_triangle(r, &(s->triangle[i]), &temp_h)) {
			if (temp_h.t < h->t ) {
				h->t = temp_h.t;
				h->m = temp_h.m;
				h->normal = temp_h.normal;
			}
			result = 1;
		}
	}
	return result;
}

float4 reflect(float4 v, float4 n)
{
	return v - 2 * dot(n, v) * n;
}

float4 trace(ray* r, scene* s)
{
	float4 factor = (float4) (1.0f, 1.0f, 1.0f, 1.0f);
	float4 result = (float4) (0.0f, 0.0f, 0.0f, 0.0f);
	ray current_ray = {r.origin, r.dir};
	hit h, temp_h;
	for (int i = 0; i < BOUNCES; i++) {
		if (!intersect_scene(&current_ray, s, &h)) break;
		float4 hit_point = current_ray.origin + h.t * current_ray.dir;
		float4 current_color = h.m->diffuse * s->ambient;
		for (int j = 0; j < s->num_lights; j++) {
			float4 light_dir = fast_normalize(s->lights[j].pos - hit_point);		
			ray light_ray = {hit_point, light_dir};
			if (!intersect_scene(&light_ray, s, &temp_h)) {
				float shade_factor = clamp(dot(h.normal, light_dir), 0, 1.0f);
				current_color += shade_factor * h.m->diffuse * s->lights[j].color;
			}
		}
		result += factor * current_color;
		factor *= h.m->specular;
		current_ray = {hit_point, reflect(current_ray.dir, h.normal)};		
	}
	if (i == 0) result = (float4) BGCOLOR;
	return result;
}

__kernel void render(__constant float *tvertex, __constant int *vindex, __constant int *tmindex, int tcount, int vcount,
										 __constant float *wvertex, __constant int *wmindex,  int wcount,
										 __constant float *sp_center, __constant float *sp_radius, __constant int *smindex, int scount,
										 __constant float *plane_d, __constant float *plane_n, __constant int *pmindex, int pcount,
										 __constant float *mdiffuse, __constant float *mspec, int mcount,
										 camera cam)
{
	triangle triangle[tcount];
	wall wall[wcount];
	sphere sphere[scount];
	plane plane[pcount];
	material mat[mcount];
	float4 tvertex4[vcount];
	for (int i = 0; i < vcount; i++) {
		tvertex4[i] = (float4) (tvertex[3 * i], tvertex[3 * i + 1], tvertex[3 * i + 2], 0);
	}
	for (int i = 0; i < mcount; i++) {
		mat[i] = {(float4) (mdiffuse[3 * i], mdiffuse[3 * i + 1], mdiffuse[3 * i + 2], 0),
							(float4) (mspec[3 * i], mspec[3 * i + 1], mspec[3 * i + 2], 0)};
	}
	for (int i = 0; i < tcount; i++) {
		float4 normal = fast_normalize(cross(tvertex[vindex[3 * i + 1]] - tvertex[vindex[3 * i]], tvertex[vindex[3 * i + 2]] - tvertex[vindex[3 * i]]));
		triangle[i] = {tvertex4 + vindex[3 * i], tvertex4 + vindex[3 * i + 1], tvertex4 + vindex[3 * i + 2], normal, mat + tmindex[i]};
	}	
}
