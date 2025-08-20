#include "definitions.h"
#include "bullet.c"

#ifndef __A2_BASE_PLAYER_H
#define __A2_BASE_PLAYER_H

enum {
    VAR_PLAYER_GUN_POSITION,
};

// predeclared to better see what is avilables
EntityModel a2_player_get_stock_model(Constants c);
void        a2_player_draw(void* e, void* c);
void        a2_player_update(
        void* e, 
        void* p, 
        void* a, 
        void* c, 
        void* s,
        void* as
);

Entity a2_create_player(Constants c) {
    Entity player = {
        .type     = A2_ENTITY_TYPE_PLAYER,
        .position = {0,0},
        .velocity = {0,0},
        .face     = {0,-1}, // up
        .rotation = 0,
        .base = a2_player_get_stock_model(c),
        .traits = 
            ETRAIT_USE_DEFAULTS + 
            ETRAIT_MOVEABLE     +
            ETRAIT_DRAGGABLE    + 
            ETRAIT_WRAPABLE
        ,
        .update = a2_player_update,
        .draw = a2_player_draw,
    };
    return player;
}

EntityModel a2_player_get_stock_model(Constants c) {
    Vector2 verts[] = {
        { 1, 1},
        {-1, 1},
        { 0,-2},
    };

    EntityModel m = {
        .fg = RAYWHITE,
        .scale = c.A2C_ENTITY_PLAYER_SCALE,
        .thickness = c.A2C_ENTITY_PLAYER_LINE_THICKNESS,
        .data = {
            [A2_MODEL_DATA_GUN1] = 2,
            [A2_MODEL_DATA_ENGINE_START] = 0,
            [A2_MODEL_DATA_ENGINE_END]   = 1,
        },
    };
    for(size_t i = 0; i < ARRAY_LEN(verts); i++) {
        a2_model_append_vert(&m, verts[i]);
    }
    return m;
}


// TODO: use named constant for recoil value
Vector2 a2_player_recoil(Entity* ent, Constants c) {
    (void) c;

    EntityModel cached = ent->cache.rotated;
    size_t gun1v = cached.data[A2_MODEL_DATA_GUN1];
    Vector2 recoil = Vector2Scale(
            Vector2Normalize(
                Vector2Subtract(ent->position, cached.poly[gun1v])
            ), 30
        );
    return recoil;
}

// TODO: use named constant
Vector2 a2_player_random_spread(Constants c) {
    (void) c;
    Vector2 spread = {
        GetRandomValue(-40,40),
        GetRandomValue(-40,40)
    };
    return spread;
}

void a2_player_system_ondeath(void* global_state) {
    GameState* s = global_state;
    assert(s);


    Pool*       ents    = &(s->entities);
    Constants   cfg     = s->config;
    Entity *it;

    (void) cfg;

    for(size_t i = 0; i < ents->max_size; i++) {
        it = a2_entpool_refer(ents, i);
        if (!it) continue;
        if (it->type == A2_ENTITY_TYPE_PLAYER)
            if(it->data.die) {
                s->runtime.state = A2_STATE_GAMEPLAYEND;
                it->base.hide = true;
            }
    }
}



void a2_player_draw(void* et, void* c) {
    Entity*         ent     = et;
    assert(c);
    Constants      vars     = *(Constants*)(c);


#if 0
    // overdraw outline and body
    EntityModel m = ent->cache.rotated;
    for(size_t i = 0; i < m.count; i++)
        DrawLineEx(
                m.poly[i    % m.count],
                m.poly[(i+1)% m.count],
                m.thickness,
        );
#endif

    // draw the engine
    // TL;DR how to
    // take two points that form a side that has the engine
    // take direction vector from being to end: by doing end - begin
    // take length of that vector and divide by two,
    // add that vector to the begin
    // boom - middle point
    Vector2 b = ent->cache.rotated.poly[ent->base.data[A2_MODEL_DATA_ENGINE_START]];
    Vector2 e = ent->cache.rotated.poly[ent->base.data[A2_MODEL_DATA_ENGINE_END]];
  
    Vector2 relative = Vector2Subtract(e,b);
    float sidelen = Vector2Length(relative);
    Vector2 normalized = Vector2Normalize(relative);
    Vector2 middle = Vector2Add(b,Vector2Scale(normalized,sidelen/2));

    ent->data.engine_position = middle;

    // make engine take only half of the face
    Vector2 engine_b, engine_e;
    engine_b = Vector2Add(b,Vector2Scale(normalized, sidelen*0.25));
    engine_e = Vector2Add(b,Vector2Scale(normalized, sidelen*0.75));


    // take face as normal vector for engine peak and inverse it
    // use dot product to draw engine only in direction against its face
    // also cool looking effect depending on angle between face and ship velocity
    float engine_len = ent->data.engine_trust; 

    Vector2 normal   = Vector2Negate(Vector2Normalize(ent->face));
    Vector2 peak = Vector2Add(middle, Vector2Scale(normal, engine_len));

    // quirky
    // check DrawTriangle comment at:
    // https://www.raylib.com/cheatsheet/cheatsheet.html
    DrawTriangle(engine_e, peak, engine_b, vars.A2C_ENTITY_PLAYER_ENGINE_COLOR); // TODO: CUSTOM ENGINE COLOR

}

void a2_player_update(void* e, void* p, void* a, void* c, void* s, void* as) {
    assert(c && e && p && a && s && as); // validate all pointers valid
    
    Entity*     ent     = e;
    Pool*       pool    = p;
    Constants   consts  = *(Constants*)c;
    SoundBuffer* sb     = s;
    Assets       assets = *(Assets*)as;

    // constant aliasing
    const float ROTATION_SPEED = 
        consts.A2C_ENTITY_PLAYER_ROTATION_SPEED;
    const float ACCELERATION = 
        consts.A2C_ENTITY_PLAYER_ACCELERATION;
    const float DECELERATION = 
        consts.A2C_ENTITY_PLAYER_DECELERATION;

    // TODO?: make player bounce and not die
    // 3 lives or something
    
    // prevent sound dupping
    // vvvvvvvvvvvvvvvv
    if (!ent->data.die) for(size_t i = 0; i < ent->collided.count; i++) {
        if(ent->collided.items[i]->type == A2_ENTITY_TYPE_ASTEROID) {
            StopSound(assets.engine);
            ent->data.die = true;
            ent->data.engine_trust = 0;
            a2_playsound(sb,assets.shipbreak);
            break;
    }}
        

    float model_scale = ent->base.scale;
    Vector2 tip = ent->base.poly[ent->base.data[A2_MODEL_DATA_GUN1]];
    Vector2 gun_position = Vector2Scale(ent->face,Vector2Length(tip)*model_scale+5); 
    //                                                                ^^^^
    // i added N to just make sure bullet never collides with player who shot it
    gun_position = Vector2Add(ent->position, gun_position);
    
    if (IsKeyPressed(KEY_UP) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        ent->data.gun_overload += 100 * GetFrameTime();
        a2_playsound(sb, assets.gun, .pitch = 0.75 + ent->data.gun_overload/10);
        
        // bullets inherit player velocity
        // bullets (not lazers) have a spread
        Entity bullet = a2_create_bullet(consts, ent->face, gun_position, ent->rotation);
        bullet.velocity = Vector2Add(bullet.velocity, ent->velocity);
        bullet.velocity = Vector2Add(bullet.velocity, a2_player_random_spread(consts));
        
        // player experiences recoil
        ent->velocity = Vector2Add(ent->velocity, a2_player_recoil(ent,consts));

        a2_entity_spawn(pool, bullet);
    }

    // decay gun overload
    ent->data.gun_overload -= 100 * GetFrameTime() * 0.125;
    ent->data.gun_overload = Clamp(ent->data.gun_overload, 0, 10);

    // decay score time
    ent->data.score_time -= GetFrameTime();
    ent->data.score_time = Clamp(ent->data.score_time, 0, 1);

    if (IsKeyDown(KEY_D)) a2_entity_rotate(e,  ROTATION_SPEED);
    if (IsKeyDown(KEY_A)) a2_entity_rotate(e, -ROTATION_SPEED);
    
    if (IsKeyDown(KEY_W)) {
        ent->acceleration = Vector2Scale(ent->face, ACCELERATION);
    } else ent->acceleration = Vector2Zero();
    if (IsKeyDown(KEY_S)) {
        Vector2 oposite = Vector2Scale(
                Vector2Normalize(ent->face), 
                -DECELERATION
        );
        ent->acceleration = Vector2Add(ent->acceleration, oposite);
    } 

    // engine
    float trust =
        (Vector2Length(ent->velocity)/20) * 
        Clamp(Vector2DotProduct(ent->velocity, ent->face),-1,1);

    if (trust && !ent->data.die) {
        // loop sound, raise its volume depending on velocity
        float vol = Clamp(trust/20,0, 0.45);
        SetSoundVolume(assets.engine, vol);
        SetSoundPitch(assets.engine, vol);
        if (!IsSoundPlaying(assets.engine))
            PlaySound(assets.engine);
    }

    ent->data.engine_trust = trust;
} 

void a2_player_trail_draw(ShaderDefinition* s, void* p, Constants* cfg) {
    enum {
        FBO_FADE_PLANE
    };

    RenderTexture2D rt = s->items[FBO_FADE_PLANE];
    Pool*   pool    = p;
    Entity* player  = 0;
    // This is complicated..
    // I don't know how to fade color with self-drawing
    // texturing, i tried to make one but it creates artifacts
    // on the screen.
    //
    // This fading is clunky, however with this specific value
    // it makes everything darker by only one 1 color tone, and actually
    // fades all the pixels
    //
    // at this specific transprency...
    Color fade_overwrite = {0,0,0,0x10}; 
    int fade_steps = 2;
    float trail_size; // model size of a player
    float vel;
    float factor;

    assert(cfg);
    Constants vars = *cfg;

    static Vector2 
        pos_now, 
        pos_before
    ;


    // find player
    for(size_t i = 0; i < pool->max_size; i++) {
        Entity* it;
        if (!(it = a2_entpool_refer(pool, i)))  continue;
        if (it->type == A2_ENTITY_TYPE_PLAYER) player = it;
    }
    // player MUST exist
    assert(player);

    trail_size = player->base.scale;
    vel = Vector2Length(player->velocity);
    factor = 500;
    // camera MUST be provided
    assert(s->cam);
    Camera2D cam = *(s->cam);


    pos_before  = pos_now;
    pos_now     = player->data.engine_position;

    Vector2 
        pos_before_screen = GetWorldToScreen2D(pos_now   , cam),
        pos_now_screen    = GetWorldToScreen2D(pos_before, cam);   

    #define HARDCODED_FBO_SIZE (Vector2){2560,1440}
    BeginTextureMode(rt);
    {
        // fade with N steps which = fade speed
        for(int i = 0; i < fade_steps; i++)
            DrawRectangleRec(rec(0,0,vec2_unfold(HARDCODED_FBO_SIZE)), fade_overwrite);
        
        if (Vector2Distance(pos_before, pos_now) < cam.offset.y-1)
            DrawLineEx(
                    pos_before_screen, 
                    pos_now_screen, 
                    Lerp(0, trail_size, vel/factor), 
                    vars.A2C_ENTITY_PLAYER_TRAIL_COLOR);
    }
    EndTextureMode();

    Shader blur = s->sd;
    BeginShaderMode(blur);
    DrawTextureRec(rt.texture, 
            rec(0,0,HARDCODED_FBO_SIZE.x,-HARDCODED_FBO_SIZE.y), 
            vec2(0,0), 
            WHITE);
    EndShaderMode();

}

const char* a2_player_blur_fragment(void) {
    return
        "#version 330\n"
        "\n"
        "in vec2 fragTexCoord;\n"
        "out vec4 finalColor;\n"
        "\n"
        "uniform sampler2D texture0;\n"
        "uniform vec2    resolution;\n"
        "uniform float   strength;\n"
        "\n"
        "const float kernel[5] = float[](0.204164, 0.304005, 0.193783, 0.091305, 0.028743);\n"
        "\n"
        "void main() {\n"
        "    vec2 tex_offset = 1.0 / resolution * strength;\n"
        "\n"
        "    // -------- Horizontal blur --------\n"
        "    vec4 horiz = texture(texture0, fragTexCoord) * kernel[0];\n"
        "    for (int i = 1; i < 5; i++) {\n"
        "        vec2 offset = vec2(tex_offset.x * i, 0.0);\n"
        "        horiz += texture(texture0, fragTexCoord + offset) * kernel[i];\n"
        "        horiz += texture(texture0, fragTexCoord - offset) * kernel[i];\n"
        "    }\n"
        "\n"
        "    // -------- Vertical blur (applied on original texture again, \n"
        "    // not on \"horiz\" because we don’t have intermediate storage) --------\n"
        "    vec4 vert = texture(texture0, fragTexCoord) * kernel[0];\n"
        "    for (int i = 1; i < 5; i++) {\n"
        "        vec2 offset = vec2(0.0, tex_offset.y * i);\n"
        "        vert += texture(texture0, fragTexCoord + offset) * kernel[i];\n"
        "        vert += texture(texture0, fragTexCoord - offset) * kernel[i];\n"
        "    }\n"
        "\n"
        "    // Combine results — approximate 2-pass blur\n"
        "    finalColor = (horiz + vert) * 0.5;\n"
        "}\n";
}

#endif//__A2_BASE_PLAYER_H
