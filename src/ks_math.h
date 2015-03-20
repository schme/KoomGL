#ifndef KS_MATH_H_
#define KS_MATH_H_

/*
 *  #define KS_MATH_IMPLEMENTATION in the file you want the
 *  implementations in before the include:
 *
 *
 *  #define KS_MATH_IMPLEMENTATION
 *  #include "ks_math.h"
 */

#define Min(a, b) (a)<(b) ? (a) : (b)
#define Max(a, b) (a)>(b) ? (a) : (b)

#include "types.h"
#include <math.h>

union vec2 {
    struct {
        r32 x, y;
    };
    r32 e[2];
};


union vec3 {
    struct {
        r32 x, y, z;
    };
    struct {
        r32 r, g, b;
    };
    r32 e[3];
};

union vec4 {
    struct {
        r32 x, y, z, w;
    };
    struct {
        r32 r, g, b, a;
    };
    r32 e[4];
};


/***********************
 *  scalar operations  *
 ***********************/

inline i32
Sign( i32 a) {
    i32 result = (a >= 0) ? 1 : -1;
    return result;
}

inline r32
Lerp( r32 A, r32 B, r32 t) {
    r32 result = (1.0f - t)*A + t*B;
    return result;
}


inline r32
Clamp( r32 minV, r32 maxV, r32 v) {
    r32 result = v;
    if( result < minV) {
        result = minV;
    } else if ( result > maxV) {
        result = maxV;
    }
    return result;
}

inline u32
RoundR32ToU32( r32 a) {
    u32 result = (u32)roundf(a);
    return result;
}


inline r32
Sqrt( r32 a) {
    r32 result = sqrtf( a);
    return result;
}


inline r32
Pow( r32 x, r32 n) {
    r32 result = powf( x, n);
    return result;
}


inline r32
Squared( r32 a) {
    r32 result = a*a;
    return result;
}


inline r32
Abs( r32 a) {
    r32 result = fabs(a);
    return result;
}


/********************
 *  vec2 operators  *
 ********************/

inline vec2
Vec2( r32 a) {
    vec2 v{
        a, a
    };
    return v;
}

inline vec2
Vec2( i32 a) {
    vec2 v = Vec2((r32)a);
    return v;
}

inline vec2
operator * ( r32 a, vec2 b) {

    vec2 v;
    v.x = a * b.x;
    v.y = a * b.y;
    return v;
}

inline vec2
operator * ( vec2 b, r32 a) {

    vec2 v = a*b;
    return v;
}

inline vec2 &
operator *= ( vec2 &b, r32 a) {

    b = a * b;
    return b;
}

inline vec2
operator - ( vec2 a) {

    vec2 v;
    v.x = -a.x;
    v.y = -a.y;
    return v;
}

inline vec2
operator + (vec2 a, vec2 b) {

    vec2 v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    return v;
}


inline vec2
operator - (vec2 a, vec2 b) {

    vec2 v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    return v;
}


inline vec2 &
operator += (vec2 &a, vec2 b) {

    a = a + b;
    return a;
}


inline r32
Dot( vec2 a, vec2 b) {

    r32 r = a.x*b.x + a.y*b.y;
    return r;
}


inline vec2
Norm( vec2 a) {
    vec2 v;
    v = (1.0f/Sqrt(a.x*a.x + a.y*a.y)) * a;
    return v;
}


/********************
 *  vec3 operators  *
 ********************/


inline vec3
Vec3( r32 a) {
    vec3 v{
        {a, a, a}
    };
    return v;
}

inline vec3
Vec3( i32 a) {
    vec3 v = Vec3((r32)a);
    return v;
}

inline vec3
Vec3( r32 x, r32 y, r32 z) {
    vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline vec3
operator * ( r32 a, vec3 b) {

    vec3 v;
    v.x = a * b.x;
    v.y = a * b.y;
    v.z = a * b.z;
    return v;
}

inline vec3
operator * ( vec3 b, r32 a) {

    vec3 v = a*b;
    return v;
}

inline vec3 &
operator *= ( vec3 &b, r32 a) {

    b = a * b;
    return b;
}

/** Todo(kasper): Find out what this is called and make a function for it */
inline vec3
operator * ( vec3 a, vec3 b) {
    vec3 v;
    v.x = a.x * b.x;
    v.y = a.y * b.y;
    v.z = a.z * b.z;
    return v;
}

inline vec3
operator - ( vec3 a) {

    vec3 v;
    v.x = -a.x;
    v.y = -a.y;
    v.z = -a.z;
    return v;
}

inline vec3
operator + (vec3 a, vec3 b) {

    vec3 v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    v.z = a.z + b.z;
    return v;
}


inline vec3
operator - (vec3 a, vec3 b) {

    vec3 v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    v.z = a.z - b.z;
    return v;
}


inline vec3 &
operator += (vec3 &a, vec3 b) {

    a = a + b;
    return a;
}


inline r32
Dot( vec3 a, vec3 b) {

    r32 r = a.x*b.x + a.y*b.y + a.z*b.z;
    return r;
}


inline vec3
Norm( vec3 v) {
    vec3 result;
    result = (1.0f/Sqrt(v.x*v.x + v.y*v.y + v.z*v.z)) * v;
    return result;
}


inline r32
LengthSq( vec3 v) {
    r32 result;
    result = Dot( v, v);
    return result;
}

inline r32
Length( vec3 v) {
    r32 result;
    result = Sqrt(LengthSq( v));
    return result;
}


inline vec3
Reflect( vec3 incident, vec3 normal) {
    vec3 result = Norm( 2.0f * normal * Dot( normal, incident) - incident);
    return result;
}


/********************
 *  vec4 operators  *
 ********************/


inline vec4
Vec4( r32 a) {
    vec4 v{
        {a, a, a, a}
    };
    return v;
}

inline vec4
Vec4( i32 a) {
    vec4 v = Vec4((r32)a);
    return v;
}

inline vec4
operator * ( r32 a, vec4 b) {

    vec4 v;
    v.x = a * b.x;
    v.y = a * b.y;
    v.z = a * b.z;
    v.w = a * b.w;
    return v;
}

inline vec4
operator * ( vec4 b, r32 a) {

    vec4 v = a*b;
    return v;
}

inline vec4 &
operator *= ( vec4 &b, r32 a) {

    b = a * b;
    return b;
}

inline vec4
operator - ( vec4 a) {

    vec4 v;
    v.x = -a.x;
    v.y = -a.y;
    v.z = -a.z;
    v.w = -a.w;
    return v;
}

inline vec4
operator + (vec4 a, vec4 b) {

    vec4 v;
    v.x = a.x + b.x;
    v.y = a.y + b.y;
    v.z = a.z + b.z;
    v.w = a.w + b.w;
    return v;
}


inline vec4
operator - (vec4 a, vec4 b) {

    vec4 v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    v.z = a.z - b.z;
    v.w = a.w - b.w;
    return v;
}


inline vec4 &
operator += (vec4 &a, vec4 b) {

    a = a + b;
    return a;
}


inline r32
Dot( vec4 a, vec4 b) {

    r32 r = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.z;
    return r;
}


inline vec4
Norm( vec4 a) {
    vec4 v;
    v = (1.0f/Sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w)) * a;
    return v;
}


#ifdef KS_MATH_IMPLEMENT

#endif
#endif // KS_MATH_H_
