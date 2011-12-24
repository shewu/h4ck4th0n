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
  float2 p1, p2, normal;
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

typedef struct {
  int width, height;
  float4 u, h, d, pos;
  float angle;
} camera;
