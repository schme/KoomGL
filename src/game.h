#ifndef GAME_H_
#define GAME_H_

#include "platform.h"

#include "ks_math.h"

static i32 buf_width = windowWidth;
static i32 buf_height = windowHeight;


static const r32 eps = 1e-5f;
static const u32 max_depth = 6;
static const r32 max_clip = 100000.0f;


struct Ray {
    vec3 v;
    vec3 pos;
    r32 length; /* sometimes used to store the last t for epsilon usage */
};

struct Plane {
    vec3 color;
    vec3 normal;
    r32 pos;
    i32 matIndex;
};

struct Sphere {
    vec3 pos;
    vec3 color;
    r32 rad;
    i32 matIndex;
};

struct Light {
    vec3 pos;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct Material {
    r32 specular;
    r32 diffuse;
    r32 ambient;
    r32 alpha;  // shininess
};

enum class HitType {
    None = 0, Plane, Sphere, Light
};

struct Intersection {
    vec3 location;
    vec3 normal;
    i32 index;
    HitType type = HitType::None;
    r32 distance = max_clip;
};

inline Intersection
InitIntrs() {
    Intersection i;
    i.location = {};
    i.normal = {};
    i.index = -1;
    i.type = HitType::None;
    i.distance = max_clip;
    return i;
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
