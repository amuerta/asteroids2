#include "depend.h"

#ifndef __UTIL_H
#define __UTIL_H


#define A2_ARRAY_LEN(A) (sizeof((A)) / sizeof((A)[0]))

#ifndef DEBUG
#   define debug(...) // __VA_ARGS__ //
#endif
#ifdef DEBUG
#   define debug(...) printf("[DEBUG] "__VA_ARGS__);
#endif

#define min(a,b) ((a)>(b)) ? (b) : (a)
#define max(a,b) ((a)<(b)) ? (b) : (a)

#define bool_fmt(expr) (expr) ? "true" : "false"
#define rec_fmt(rec) rec.x, rec.y, rec.width, rec.height
#define vec2_fmt(v) v.x, v.y
#define vec2_unfold(v) v.x, v.y

static inline Vector2 vec2(float x, float y) {
    return (Vector2) {x,y};
}

static inline Vector2 vec2dim(Rectangle rec) {
    return (Vector2) {rec.width,rec.height};
}

static inline Rectangle rec(float x, float y, float w, float h) {
    return (Rectangle) {x,y,w,h};
}

static inline bool float_approx_eq(float a, float b, float eps) {
    return fabs(a - b) < eps;
}

static inline Rectangle rec_expand(Rectangle rec, float size) {
    Rectangle r = {
        .x      = rec.x - size,
        .y      = rec.y - size,
        .width  = rec.width  + 2*size,
        .height = rec.height + 2*size
    };
    return r;
}

static inline Color color(float r, float g, float b, float a) {
    return (Color) {r,g,b,a};
}

static inline Color col(float r, float g, float b) {
    return (Color) {r,g,b,255};
}

static inline float ease_out_quint(float n) {
    return 1 - pow(1 - n, 5);
}

#endif//__UTIL_H
