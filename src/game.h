#ifndef GAME_H_
#define GAME_H_

#include "platform.h"

#include "ks_math.h"

static i32 buf_width = windowWidth;
static i32 buf_height = windowHeight;

static u32 numPlanes = 1;
static u32 numSpheres = 2;
static u32 numLights = 1;

static const r32 eps = 1e-5f;
static const u32 max_depth = 6;
static const r32 max_clip = 100000.0f;


struct Ray {
    vec3 v;
    vec3 pos;
    r32 length;
};

struct Plane {
    vec3 color;
    vec3 normal;
    r32 pos;
};

struct Sphere {
    vec3 pos;
    vec3 color;
    r32 rad;
};

struct Light {
    vec3 pos;
    vec3 color;
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

static Plane *planes;
static Sphere *spheres;
static Light *lights;

#endif // GAME_H_
