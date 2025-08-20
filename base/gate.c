#include "definitions.h"

#ifndef __A2_BASE_GATE_H
#define __A2_BASE_GATE_H


void a2_gate_draw(void *e, void* c);

Entity a2_create_gate(Constants c) {
    (void) c;
    Entity player = {
        .type     = A2_ENTITY_TYPE_GATE,
        .position = {0,0},
        .velocity = {0,0},
        .face     = {0,-1}, // up
        .rotation = 0,
        .base = {0}, // has no model, uses a2_gate_draw in a2_update_intro
        .traits = 
            ETRAIT_USE_DEFAULTS + 
            ETRAIT_MOVEABLE     +
            ETRAIT_DRAGGABLE    + 
            ETRAIT_WRAPABLE
        ,
        .draw = a2_gate_draw,
    };
    return player;
}


void a2_gate_draw(void *e, void* c) {
    Entity* ent = e;
    Vector2 pos =  ent->position;
    Constants cfg = *(Constants*)c;

    // TODO: add constants for line and color
    float len = Lerp(0, 50, ease_out_quint(Clamp(ent->data.timer1, 0, 1)));

    Vector2 
        l = vec2(pos.x - len, pos.y), 
        r = vec2(pos.x + len, pos.y) ;


    DrawRectangleRec(rec(pos.x-len, pos.y, len*2, len*10), cfg.A2C_WINDOW_BACKGROUND_COLOR);
    DrawLineEx(pos, l, 3, RAYWHITE);
    DrawLineEx(pos, r, 3, RAYWHITE);

}

#endif//__A2_BASE_GATE_H
