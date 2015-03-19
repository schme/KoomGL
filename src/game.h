#ifndef GAME_H_
#define GAME_H_

#include "platform.h"

#include "ks_math.h"

static i32 buf_width = windowWidth;
static i32 buf_height = windowHeight;


static const r32 eps = 1e-5f;
static const u32 max_recursion_depth = 6;
static const r32 max_clip = 100000.0f;
static const r32 transmission_coefficient = 0.8f;


struct Ray {
    vec3 *pixel;
    vec3 attenuation;
    vec3 dir;
    vec3 pos;
    i32 rec_depth;
};

struct Plane {
    vec3 normal;
    r32 pos;
    i32 matIndex;
};

struct Sphere {
    vec3 pos;
    r32 rad;
    i32 matIndex;
};

struct Light {
    vec3 pos;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct Material {
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    r32 alpha;  // shininess
};

enum class HitType {
    None = 0, Plane, Sphere, Light
};

struct Intersection {
    vec3 point;
    vec3 normal;
    r32 distance = max_clip;
};

struct Hit {
    i32 index;
    HitType type;
};

inline Intersection
InitIntersection() {
    Intersection i;
    i.point = {};
    i.normal = {};
    i.distance = max_clip;
    return i;
}

inline Hit
InitHit() {
    Hit hit;
    hit.index = -1;
    hit.type = HitType::None;
    return hit;
}

static Material *materials;
static Plane *planes;
static Sphere *spheres;
static Light *lights;

static u32 numMaterials;
static u32 numPlanes;
static u32 numSpheres;
static u32 numLights;

#endif // GAME_H_
