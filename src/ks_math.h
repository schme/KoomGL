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

//TODO(kasper): Get rid of this
#include <math.h>


#include "types.h"

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


/********************
 *  vec2 operators  *
 ********************/

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


/********************
 *  vec3 operators  *
 ********************/


/********************
 *  vec4 operators  *
 ********************/



#ifdef KS_MATH_IMPLEMENT

#endif
#endif // KS_MATH_H_
