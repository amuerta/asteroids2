#ifndef __A2_GAME_DEFINITION_H
#define __A2_GAME_DEFINITION_H

#include "depend.h"
#include "../assets/assets.c"
#include "sound.c"
#include "../data.c"

typedef struct {
    struct {
        bool                terminate;
        bool                refresh;
        bool                resize;
        bool                fullscreen;
        int                 state;
        
        long long int       tick;
        int                 target_fps;
    } runtime;

    Camera2D            camera;
    ShaderSystem        shaders;

    Rectangle           *arena; // Rectangle[2]
    Constants           config;
    Pool                entities;
    Systems             systems;
    Assets              assets;
    SoundBuffer         sounds;
    mu_Context          uictx;
    // user defined variables
    GlobalData          data;
} GameState;

#endif//__A2_GAME_DEFINITION_H
