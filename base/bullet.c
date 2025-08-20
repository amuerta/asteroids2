
#ifndef __A2_BASE_BULLET_H
#define __A2_BASE_BULLET_H

#include "asteroids.c"

// predeclared to better see what is avilables
EntityModel a2_bullet_get_stock_model(Constants c);
void        a2_bullet_update(
        void* e, 
        void* p, 
        void* a, 
        void* c, 
        void* s,
        void* as
);

Entity a2_create_bullet(Constants c, Vector2 direction, Vector2 source, float rotation) {
    direction = Vector2Normalize(direction); // normalize
    Entity player = {
        .type     = A2_ENTITY_TYPE_BULLET,
        .position = source,
        .velocity = Vector2Scale(direction, 700),
        .face     = direction, // up
        .rotation = rotation,
        .base     = a2_bullet_get_stock_model(c),
        .traits   = 
            ETRAIT_USE_DEFAULTS + 
            ETRAIT_MOVEABLE     
        ,
        .update = a2_bullet_update,
    };
    return player;
}

EntityModel a2_bullet_get_stock_model(Constants c) {
    Vector2 verts[] = {
        { 0, 1 },
        { 0,-1 },
    };

    EntityModel m = {
        .fg = c.A2C_ENTITY_BULLET_COLOR,
        .scale = 10,//a2_const_get_float(c, CONSTANT_ENTITY_PLAYER_SCALE),
        .thickness = c.A2C_ENTITY_BULLET_LINE_THICKNESS,
    };
    for(size_t i = 0; i < ARRAY_LEN(verts); i++) {
        a2_model_append_vert(&m, verts[i]);
    }
    return m;
}


void a2_bullet_update(void* e, void* p, void* a, void* c, void* s, void* as) {
    assert(c && e && p && a && s && as); // validate all pointers valid
    static Entity* player = 0;

    Entity*      ent    = e;
    Pool*        pool   = p;
    Rectangle*  arenas  = a;
    Constants   consts  = *(Constants*)c;
    SoundBuffer* sb     = s;
    Assets       assets = *(Assets*)as;

    int* health = 0;
    int hits = 0;

    if(!player) for(size_t i = 0; i < pool->max_size; i++) {
        Entity* it = a2_entpool_refer(pool, i);
        if (it && it->type == A2_ENTITY_TYPE_PLAYER) 
            player = it;
    }

    if(!CheckCollisionPointRec(ent->position,arenas[ARENA_VISIBLE])) {
        a2_entity_despawn(pool, ent);
        return;
    }

    float volume;
    float pitch;

    // handle bullet collision
    for(size_t i = 0; i < ent->collided.count; i++) {
        Entity* collided = ent->collided.items[i];
    
        switch(collided->type) {
            case A2_ENTITY_TYPE_ASTEROID:
                health = &(collided->data.hp);

#define         randflt(l,r) ((float)GetRandomValue(l,r))

                Sound hurt = assets.rock;

                // TODO: pitch is not working
                if (*health > 1) {
                    volume = GetRandomValue(2,4)/10.f;
                    pitch =  GetRandomValue(4,6)/5.f;
                    collided->data.hurt_time = 1.0f;
                    *health -= 1;
                }
                else {
                    player->data.score_time += 1.0f;
                    player->data.score += collided->data.asteroid_type+1;
                        //Clamp(collided->data.asteroid_type+1, 0, A2_ASTEROID_BIG+1);
                    a2_asteroid_split(consts, pool, collided, 25);
                    //SetSoundPitch(hurt,  GetRandomValue(2,5)/10.f);
                    volume = GetRandomValue(6,8)/10.f;
                    if (collided->data.asteroid_type != A2_ASTEROID_SMALL)
                        pitch =  GetRandomValue(2,3)/5.f;
                }

                a2_playsound(sb, hurt, .pitch = pitch, .volume = volume);
                hits++;
                break;
        }
    }
#undef randflt
    if (hits > 0) a2_entity_despawn(pool, ent);
} 



#endif//__A2_BASE_PLAYER_H
