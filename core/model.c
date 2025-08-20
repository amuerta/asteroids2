#include "depend.h"

#ifndef __MODEL_H
#define __MODEL_H

// TODO: make this dynamic if needed
#define MAX_POLY_COUNT 32

typedef struct {
    int         id;
    Vector2     poly [MAX_POLY_COUNT];
    float       scale;
    float       thickness;
    size_t      count;
    Color       bg, fg, ac;
    size_t      data [MAX_SLOTS];
    bool        hide;
} EntityModel;

bool a2_model_append_vert(EntityModel* m, Vector2 point) {
    if (m->count >= MAX_POLY_COUNT) return false;
    m->poly[m->count++] = point;
    return true;
}

bool a2_model_check_collision(Vector2 point, EntityModel m) {
    return CheckCollisionPointPoly(point, m.poly, m.count);
}
  
// TODO: cache this instead of recalculating each frame for each entity.
EntityModel a2_model_transform(EntityModel m, float rotation, float scale) {
    for(size_t i = 0; i < m.count; i++) {
        Vector2 initial = m.poly[i];
        Vector2 rotated = Vector2Rotate(initial, rotation*DEG2RAD);
        Vector2 scaled  = Vector2Scale (rotated, scale);
        m.poly[i] = scaled;
    }
    return m;
}

// TODO: improve performance if needed.
bool a2_models_check_collision(EntityModel l, EntityModel r) {
    for(size_t i = 0; i < l.count; i++)
        if(CheckCollisionPointPoly(l.poly[i], r.poly, r.count)) 
            return true;
    if (l.count == 0 || r.count == 0)
        return false;
    return false;
}

#endif//__MODEL_H
