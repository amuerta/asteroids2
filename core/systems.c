#include "types.h"
#include "gamedef.h"


#ifndef __A2_SYSTEMS_H
#define __A2_SYSTEMS_H

// api
void a2_append_system(void* gs, SystemFn system) {
    GameState* s = gs;
    Systems* sys = &(s->systems);
    da_append(sys, system);
}

// default systems

void a2_system_up_collision    (GameState* s) {
    Entity *it, *other;
    Pool* ents = &(s->entities);

    // reset collision at beginning of each frame
    for(size_t i = 0; i < ents->max_size; i++) {
        it = a2_entpool_refer(ents, i);
        if (it) it->collided.count = 0;
    }

    for(size_t i = 0; i < ents->max_size; i++) {
        for(size_t j = 0; j < ents->max_size; j++) {
            if (i == j) continue;
            it      = a2_entpool_refer(ents, i);
            other   = a2_entpool_refer(ents, j);
            if (!it || !other) continue;

            // if collision 
            EntityModel 
                l = it->cache.rotated, 
                r = other->cache.rotated;
            if (a2_models_check_collision(l,r)) {
                // append to collided list
                da_append(&(it->collided), other);
                da_append(&(other->collided), it);
                //li_append(it->next, *other);
                //li_append(other->next, *it);
            }

        }
    }
}

#endif//__A2_SYSTEMS_H
