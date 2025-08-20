
// global debug variable
#ifdef DEBUG
static bool debug = true;
#endif

#include "assets/assets.c"
#include "base/base.h"
#include "init.c"
#include "tick.c"


#ifndef __GAME_H
#define __GAME_H


void        a2_initilize                (GameState*);
void        a2_update                   (GameState*);
void        a2_draw                     (GameState*);
void        a2_deinitilize              (GameState*);
Rectangle*  a2_arena_from_screen        (Vector2);

void a2_initilize(GameState* s) {
    const Vector2 window_size = {800,600};
    s->runtime.state  = A2_STATE_MENU;
    s->config = a2_constants_default();
    s->arena  = a2_arena_from_screen(window_size);
    s->assets = a2_assets_get();
    mu_rl_init(&(s->uictx));
    
    a2_systems_init(s);
    a2_entities_init(&(s->entities),s->config);
    a2_shaders_init(s);


    // set style
    a2_ui_set_style(&(s->uictx),"windowbg", GetColor(0x151515FF));
    a2_ui_set_style(&(s->uictx),"titlebg",  GetColor(0x101010FF));
    a2_ui_set_style(&(s->uictx),"button",   GetColor(0x101010FF));
    a2_ui_set_style(&(s->uictx),"border",   GetColor(0x202020FF));
}

void a2_deinitilize(GameState* s) {
    
    for(size_t i = 0; i < s->entities.max_size; i++) {
        free(s->entities.buffer[i].as_entity.collided.items);
    } 

    for(size_t i = 0; i < s->shaders.count; i++) {
        ShaderDefinition def = s->shaders.items[i];
        // unload shader 
        UnloadShader(def.sd);
        // unload Frame Buffer Objects
        for(size_t fbos = 0; fbos < def.count; fbos++)
            UnloadRenderTexture(def.items[fbos]);
    }
    
    free(s->systems.items);
    free(s->shaders.items);
    //a2_assets_unload(&(s->assets));
}

void a2_ui(GameState* s) {
    Rectangle screen = {
        0,0,
        GetScreenWidth(), 
        GetScreenHeight()
    };

    mu_Context* ctx = &(s->uictx);
    mu_begin(ctx);

    // update windows:
    if (s->runtime.resize) {
        // Debug window
        mu_Container* debug_win = mu_get_container(ctx, "Debug");
        if (debug_win) {
            debug_win->rect.y = 0;
            debug_win->rect.x = (screen.x + screen.width) - debug_win->rect.w;
        }

        // Settings window
        mu_Container* sett = mu_get_container(ctx, "Settings");
        if (sett) {
            sett->rect.x = screen.x + (screen.width - sett->rect.w)/2;
            sett->rect.y = screen.y + (screen.height - sett->rect.h)/2;
        }
    }

#ifdef DEBUG
    if (debug) a2_base_draw_debug_window(s);
#endif

  
    a2_tick_ui(s);

    mu_end(ctx);
    mu_rl_flush(ctx);
    DrawRectangleLinesEx(rec(0,0,s->arena[0].width,s->arena[0].height), 1, DARKGRAY);
}


void a2_refresh(GameState* s) {
    // Settings
    SetMasterVolume(s->data.settings.volume);
    
    // Entity
    s->config.A2C_ENTITY_BULLET_COLOR           = s->data.settings.bullet_color;
    s->config.A2C_ENTITY_PLAYER_ENGINE_COLOR    = s->data.settings.engine_color;
    s->config.A2C_ENTITY_PLAYER_TRAIL_COLOR     = s->data.settings.trail_color;
    
    s->runtime.refresh = false;
}

void a2_time_inter_tick(GameState* s) {
    s->runtime.tick++;
}

// TODO: list of user windows, make it part of CORE 
void a2_update(GameState* s) {
    // static storage
    static Vector2 screen_size;
    Vector2 real_screen = {
        GetScreenWidth(), 
        GetScreenHeight()
    };

    enum { ARENA_VISIBLE, ARENA_TOTAL };

    // aliasing
    //mu_Context* ctx     = &(s->uictx);
    //Constants c         = s->config;
    //Pool* ents          = &(s->entities);
    //Entity* ent         = 0;
    //Rectangle screen    = s->arena[ARENA_VISIBLE];

#ifdef DEBUG
    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_SLASH)) debug = !debug;
#endif

    // update screen
    if (!Vector2Equals(screen_size, real_screen)) {
        s->arena = a2_arena_from_screen(real_screen);
        s->runtime.resize = true;
    } else s->runtime.resize = false;

    // update ui events
    mu_rl_events(&(s->uictx));

    // run user state update 
    a2_tick_update(s);

    //memcpy(&screen_size, &(s->arena[ARENA_VISIBLE]), sizeof(screen_size));
    
    a2_time_inter_tick(s);
    
    screen_size.x = s->arena[ARENA_VISIBLE].width;
    screen_size.y = s->arena[ARENA_VISIBLE].height;
}

void a2_draw(GameState* s)  {
    Constants       c       = s->config;
    ShaderSystem    shaders = s->shaders;
    Pool*           ents    = &(s->entities);
    Entity*         ent     = 0;
    //mu_Context*     ctx = &(s->uictx);
    Camera2D    cam = {
        .offset = {
            (s->arena[ARENA_VISIBLE].width/2),
            (s->arena[ARENA_VISIBLE].height/2)
        },
        .target = {0,0},
        .zoom = 1.0f,
        .rotation = 0.0f
    }; 

    // update the camera
    s->camera = cam;

    BeginDrawing(); 
    {
     
        ClearBackground(c.A2C_WINDOW_BACKGROUND_COLOR);
        for(size_t i = 0; i < shaders.count; i++) {
            ShaderDefinition* self = &shaders.items[i];
            shaders.items[i].run(self, ents, &c);
        }

        // gameplay render
        BeginMode2D(cam);
        for(size_t i = 0; i < ents->max_size; i++) {
            if ((ent = a2_entpool_refer(ents, i))) 
                a2_entity_draw(ent, &c);
        }
        EndMode2D();

        // ui
        a2_ui(s);

    }
    EndDrawing();
}

Rectangle* a2_arena_from_screen(Vector2 screen_size) {
    static Rectangle buffer[2];
    Rectangle data[] = {
        {
            -screen_size.x/2,
            -screen_size.y/2,
            screen_size.x,
            screen_size.y,
        },
        {
            -screen_size.x,
            -screen_size.y,
            screen_size.x*2,
            screen_size.y*2,
        }
    };
    memcpy(buffer,data,sizeof(data));
    return buffer;
}

#endif//__GAME_H
