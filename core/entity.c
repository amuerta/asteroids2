#include "depend.h"
#include "types.h"
#include "constants.c"
#include "../assets/assets.c"

#ifndef __ENTITY_H
#define __ENTITY_H

enum {
    ARENA_VISIBLE,
    ARENA_TOTAL,
} ArenaKind;

typedef enum {
    ETRAIT_USE_DEFAULTS     = 1,
    ETRAIT_MOVEABLE         = 2,
    ETRAIT_GRAVITATED       = 4,
    ETRAIT_DRAGGABLE        = 8,
    ETRAIT_WRAPABLE         = 16,
    ETRAIT_COLLIDABLE       = 32,
} EntityTraits;

void a2_entity_update (
        Entity*         self, 
        Pool*           others,  
        Rectangle       area[2], 
        Constants*      config,
        SoundBuffer*    sound,
        Assets*         as
);

// default physics
void a2__entity_up_position     (Entity* e, Constants* c);
void a2__entity_up_velocity     (Entity* e, Constants* c);
void a2__entity_up_drag         (Entity* e, Constants* c);
void a2__entity_up_wrap         (Entity* e, Constants* c, Rectangle area  );
void a2__entity_up_gravity      (Entity* e, Constants* c, Pool* all       );
void a2__entity_up_collision    (Entity* e, Constants* c, Pool* all       );



void a2_entity_rotate(Entity* e, float angle);

void a2_entity_update(
        Entity*         e, 
        Pool*           p,  
        Rectangle       a[2],
        Constants*      c,
        SoundBuffer*    sb,
        Assets*         as
    ) 
{
    // run set of avilable traits
    // like pyhsics and collision
    int traits = e->traits;

    if (traits & ETRAIT_MOVEABLE) {
        a2__entity_up_position(e,c);
        a2__entity_up_velocity(e,c);
    }
    if (traits & ETRAIT_DRAGGABLE)
        a2__entity_up_drag(e,c);

    if (traits & ETRAIT_WRAPABLE)
        a2__entity_up_wrap(e,c, a[ARENA_VISIBLE]);

    // update model under rotation
    e->cache.rotated = a2_model_transform(
            e->base, 
            e->rotation, 
            e->base.scale
    );
    for(size_t i = 0; i < e->cache.rotated.count; i++) {
        Vector2* vert = e->cache.rotated.poly + i;
        *vert = Vector2Add(e->position,*vert);
    }
    
    // run binded logic
    if (e->update) e->update(e, p, a, c, sb, as);
}


void a2_entity_draw_debug(Entity* e, Constants* c) {
    const float face_debug = c->A2C_DEBUG_FACE_LING;
    
#ifdef DEBUG
    if(debug) {
#else
    if (0){
#endif 
        DrawCircleV(e->position, 2, RED);
        DrawLineV(
                e->position, 
                Vector2Add(e->position, Vector2Scale(e->face,face_debug)),
                RAYWHITE
        ); 
    }
}


void a2_entity_draw(Entity* e, Constants* c) {
    assert(c);
    Constants vars = *(Constants*)c;
    int traits = e->traits;
    

    // DEBUG
    Color linecolor = e->base.fg;
#ifdef DEBUG
    if(debug && e->collided.count) linecolor = RED;
#endif 
    
    if (e->base.hide) return;

    if (traits & ETRAIT_USE_DEFAULTS) {
        EntityModel model = e->cache.rotated;
        // draw convex polygon
        for(size_t i = 0; i < model.count; i++) 
            if (model.count >= 3) 
                DrawTriangle(
                        model.poly[i],
                        model.poly[0],
                        model.poly[(i+1) % model.count],
                    vars.A2C_WINDOW_BACKGROUND_COLOR);


        for(size_t i = 0; i < model.count; i++) {
            Vector2 curr, next;// next2;
            curr = model.poly[i];
            next = model.poly[(i + 1) % model.count];
            //next2 = model.poly[(i + 2) % model.count];
            DrawLineEx(curr,next, model.thickness, linecolor);
        }
    }

    a2_entity_draw_debug(e, c);
    // run binded logic
    if (e->draw) e->draw(e,c);
}


//
// DEFUALT TRAITS OF THE ENTITY
//
void a2__entity_up_position     (Entity* e, Constants* c) {
    A2_UNUSED(c);
    e->position = Vector2Add(
            e->position, 
            Vector2Scale(e->velocity, GetFrameTime())
    );
}

void a2__entity_up_velocity     (Entity* e, Constants* c) {
    const float threshold = c->A2C_PHYSICS_VEL_STOP_THRESHOLD;
    e->velocity = Vector2Add(e->velocity, e->acceleration);
    if (fabs(e->velocity.x) <= threshold) e->velocity.x = 0;
    if (fabs(e->velocity.y) <= threshold) e->velocity.y = 0;
}

void a2__entity_up_drag         (Entity* e, Constants* c) {
    const float drag = c->A2C_PHYSICS_DRAG_COEF;
    e->velocity = Vector2Scale(e->velocity, (1.0-drag));
}

void a2__entity_up_wrap         (Entity* e, Constants *c, Rectangle a) {
    (void) c;
    Vector2 inverse = {-1,-1};
    if (!CheckCollisionPointRec(e->position, a)) {
        e->position = Vector2Multiply(e->position, inverse);
    }
}

#if 0// TODO:
void a2__entity_up_gravity      (Entity* e, Constants *c, Pool* all) {
}
#endif


// 
// Entity manipulation
//

void a2_entity_rotate(Entity* e, float angle) {
    angle       *= GetFrameTime();
    e->rotation += angle;
    e->face     =  Vector2Rotate(e->face, angle*DEG2RAD);

    // TODO: recompute the model
}

// 
// Query
//

// TODO:
/*
Entity* a2_entity_filter_collision_list_by_type(Entity* ent, int type) {
}
*/

#endif//__ENTITY_H
