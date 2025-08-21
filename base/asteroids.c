#include "definitions.h"

#ifndef __A2_BASE_ASTEROIDS_H
#define __A2_BASE_ASTEROIDS_H


EntityModel a2_asteroid_get_stock_model(Constants c);
void        a2_asteroid_update(
        void* e,
        void* p,
        void* a,
        void* c,
        void* s,
        void* as
    );

#define range(n,b,e) ((b) >= (n) && (n) <= (e))

int a2_asteroid_get_type(Constants c) {
    (void) c;
    int rng = GetRandomValue(0,100);
    if (range(rng,0 ,30 )) return A2_ASTEROID_SMALL;
    else if (range(rng,30,85 )) return A2_ASTEROID_MEDIUM;
    else if (range(rng,85,100)) return A2_ASTEROID_BIG;
    else return A2_ASTEROID_SMALL;
}

EntityModel a2_asteroid_get_random_model(Constants c, int asteroid_kind) {
    float scale;
    float asteroid_scaler = 2;
    float verts = 4;
    switch(asteroid_kind) {
        case A2_ASTEROID_SMALL:
            scale = 15;
            verts = GetRandomValue(3,5);
            break;

        default:
        case A2_ASTEROID_MEDIUM:
            scale = 25;
            verts = GetRandomValue(6,10);
            break;

        case A2_ASTEROID_BIG:
            scale = 45;
            verts = GetRandomValue(10,14);
            break;
    }

    EntityModel m = {
        .fg = GRAY,
        .ac = ColorLerp(LIGHTGRAY, GetColor(0xf4b08bFF), 0.25),
        .scale = scale,
        .thickness = c.A2C_ENTITY_PLAYER_LINE_THICKNESS
    };

    Vector2 unit_vec = {0,-1}; // up
    int vert_count = GetRandomValue(4,verts);
    float step = 360/vert_count;
    for(int i = 0; i < vert_count; i++) {
        float random_scaler = GetRandomValue(1,asteroid_scaler);
        Vector2 point = Vector2Scale(unit_vec, random_scaler);

        a2_model_append_vert(&m, point);
        unit_vec = Vector2Rotate(unit_vec, step * DEG2RAD );
    }
    
    return m;
}


void a2_asteroid_update(void* e, void* p, void* a, void* c, void* s, void* as) {
    assert(c && e && p && a && s && as); // validate all pointers valid
 
    enum {
        ARENA_VISIBLE,
        ARENA_TOTAL,
    };

    Pool* pool = p;
    Entity* ent = e;
    //Constants consts = *(Constants*)c;
    Rectangle* arenas = a;

    ent->data.hurt_time -= GetFrameTime();
    ent->data.hurt_time = Clamp(ent->data.hurt_time, 0, 1);

    if(!CheckCollisionPointRec(ent->position,arenas[ARENA_TOTAL])) 
        a2_entity_despawn(pool, ent);
}

void a2_asteroid_draw(void *e, void* c) {
    (void) c;

    Entity* ent = e;
    int lines = 0;
    int* type = &ent->data.asteroid_type;
    int* hp = &ent->data.hp;
    assert(type && hp);

    EntityModel m = ent->cache.rotated;


    // TODO: replaced numbers by named constants
    switch(*type) {
        case A2_ASTEROID_SMALL: 
            lines = 1 - *hp;
            break;

        default:
        case A2_ASTEROID_MEDIUM: 
            lines = 3 - *hp;
            break;
        
        case A2_ASTEROID_BIG: 
            lines = 6 - *hp;
            break;
    }

    for(int i = 0; i < lines; i++) {
        DrawLineV(
                m.poly[i    % m.count],
                m.poly[(i+4)% m.count],
                m.fg
        );
    }

    // overdraw outline on damage
    for(size_t i = 0; i < m.count; i++)
        DrawLineEx(
                m.poly[i    % m.count],
                m.poly[(i+1)% m.count],
                m.thickness,
                ColorLerp(m.fg, m.ac, ent->data.hurt_time)
        );
}

Entity a2_create_asteroid(Constants c, int astertype) {
    float speed = 50;
    float hurt_time = 0;
    int health = 1;

    switch(astertype) {
        case A2_ASTEROID_SMALL:  
            speed = GetRandomValue(190,300); 
            health = 1;
            break;

        default:
            astertype = A2_ASTEROID_MEDIUM;
            // fall through
        case A2_ASTEROID_MEDIUM: 
            speed = GetRandomValue(90,120);
            health = 3;
            break;
        case A2_ASTEROID_BIG:    
            speed = GetRandomValue(30,60);
            health = 6;
            break;
    }

    Entity asteroid = {
        .type = A2_ENTITY_TYPE_ASTEROID,
        .position = {-100,0},
        .velocity = {speed,0},
        .face     = {0,-1}, // up
        .rotation = 0,
        .base = a2_asteroid_get_random_model(c, astertype),
        .traits = 
            ETRAIT_USE_DEFAULTS + 
            ETRAIT_MOVEABLE     
            //ETRAIT_DRAGGABLE    + 
            //ETRAIT_WRAPABLE
        ,
        .update = a2_asteroid_update,
        .draw = a2_asteroid_draw
    };

    // append data IN ORDER to what is defined 
    // in asteroid data enum in file: definitions.h
    asteroid.data.asteroid_type = astertype;
    asteroid.data.hp = health;
    asteroid.data.hurt_time = hurt_time;

    return asteroid;
}


void a2_asteroid_point_at_player(Entity* player, Entity* asteroid) {
    // set asteroid vector to point in player direction
    float vel = Vector2Length(asteroid->velocity);
    Vector2 vec_to_player = Vector2Subtract(player->position,asteroid->position);
    vec_to_player = Vector2Normalize(vec_to_player);
    // apply random spread 
    vec_to_player = Vector2Rotate(vec_to_player, GetRandomValue(-25,25)*DEG2RAD);
    asteroid->velocity = Vector2Scale(vec_to_player, vel);
}

Entity a2_create_asteroid_random(Constants c, Rectangle arena, Entity* player) {
        enum { TOP, RIGHT, BOTTOM, LEFT };

        Vector2 position = {0};                   
        Rectangle t     = rec_expand(arena, 100); //TODO: replace w named constant
        Entity asteroid = a2_create_asteroid(c, a2_asteroid_get_type(c));

        // set asteroid position somewhere alongside the screen edge rectangle
        // but make sure its within zone where it doesn't despawn
        switch(GetRandomValue(0,3)) {
            case TOP: 
                    position.y = t.y;
                    position.x = t.x + GetRandomValue(0, t.width);
                 break;
            case BOTTOM:
                    position.y = t.y + t.height;
                    position.x = t.x + GetRandomValue(0, t.width);
                 break;

            case RIGHT: 
                    position.y = t.y + GetRandomValue(0, t.height);
                    position.x = t.x + t.width;
                 break;

            case LEFT: 
                    position.y = t.y + GetRandomValue(0, t.height);
                    position.x = t.x;
                 break;
            default: break;
        }

        asteroid.position = position;
 
        a2_asteroid_point_at_player(player, &asteroid);

        // random rotation
        asteroid.rotation         = GetRandomValue(0,360);
        asteroid.angular_velocity = GetRandomValue(-100,100);

        return asteroid;
}

void a2_asteroid_split(Constants c, Pool* pool, Entity* source, float base_angle) {
    float length = Vector2Length(source->velocity);
    Vector2 v = Vector2Normalize(source->velocity);
    Vector2 
        l = Vector2Rotate(v,-base_angle * DEG2RAD),
        r = Vector2Rotate(v, base_angle * DEG2RAD)
    ;
    Entity la, ra;

    int* type = &(source->data.asteroid_type);

    // TODO: velocity changes with sharpnel size
    switch(*type) {
        case A2_ASTEROID_SMALL: 
            break; 

        case A2_ASTEROID_MEDIUM: 
            {
                 la = a2_create_asteroid(c, A2_ASTEROID_SMALL);
                 ra = a2_create_asteroid(c, A2_ASTEROID_SMALL);
                 la.position = source->position;
                 ra.position = source->position;
                 la.velocity = Vector2Scale(l, length);
                 ra.velocity = Vector2Scale(r, length);
                 a2_entity_spawn(pool, la);
                 a2_entity_spawn(pool, ra);
            }
            break; 

        case A2_ASTEROID_BIG: 
            {
                 la = a2_create_asteroid(c, A2_ASTEROID_MEDIUM);
                 ra = a2_create_asteroid(c, A2_ASTEROID_MEDIUM);
                 la.position = source->position;
                 ra.position = source->position;
                 la.velocity = Vector2Scale(l, length);
                 ra.velocity = Vector2Scale(r, length);
                 a2_entity_spawn(pool, la);
                 a2_entity_spawn(pool, ra);
            }
            break; 
        default: break;
    }

    a2_entity_despawn(pool, source);
}

void a2_asteroid_system_spawn(void* global_state) {
    GameState* s = global_state;
    assert(s);

    Entity 
        *player = 0,
        *it     = 0
    ;
    size_t 
        asteroid_count = 0,
        ent_count = 0
    ;


    Pool*       ents    = &(s->entities);
    Constants   cfg     = s->config;
    Rectangle*  arena   = s->arena;

    // get player, calculate asteroids
    for(size_t i = 0; i < ents->max_size; i++) {
        it = a2_entpool_refer(ents, i);
        if (!it) continue;
        
        if (it->type == A2_ENTITY_TYPE_PLAYER) 
            player = it;
        else if (it->type == A2_ENTITY_TYPE_ASTEROID)
            asteroid_count++;
        ent_count++;
    }

    // must exist
    assert(player);

    // for 800x600 max count will be 40
    // for 1920x1080 max will be 72
    size_t dynamic_spawn_count = arena[ARENA_VISIBLE].height/(10*1.5);
    const size_t max_asteroids = 64; // TODO: replace w named constant

    // TODO: remove magic numbers, place named constants in their place
    size_t  spawn_wave_size = a2_math_square_fallof(asteroid_count, 3, 4);
    bool can_spawn_wave = 
            (ent_count + spawn_wave_size < dynamic_spawn_count) &&
            (ent_count + spawn_wave_size < MAX_ENTITY_COUNT) &&
            (spawn_wave_size < max_asteroids)
        ;
    // TODO: make a better clock and add singular stray asteroids
    // spawns 
    bool spawn_tick_many = s->runtime.tick % (long int)(60 * 4.0) == 0;
    bool spawn_tick_one  = s->runtime.tick % (long int)(60 * 2.5) == 0;

    // spawn one more frequently in non even timing
    if (can_spawn_wave && spawn_tick_one) {
        Entity asteroid = a2_create_asteroid_random(
                cfg, 
                s->arena[ARENA_VISIBLE],
                player);
        a2_entity_spawn(ents, asteroid);
    }
    // spawn group on tick
    if (can_spawn_wave && spawn_tick_many) for(size_t i = 0; i < spawn_wave_size; i++ ) {
        Entity asteroid = a2_create_asteroid_random(
                cfg, 
                s->arena[ARENA_VISIBLE],
                player);
        a2_entity_spawn(ents, asteroid);
    }
}



#endif//__A2_BASE_ASTEROIDS_H
