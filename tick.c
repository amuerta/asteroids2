#ifndef __A2_TICK_H
#define __A2_TICK_H

#include "base/base.h"

void a2_tick_update(GameState* s) {
    float duration = s->config.A2C_TIMER_DURATION_INTRO;

    // TODO:
    if (IsKeyPressed(KEY_F11)) {
        s->runtime.fullscreen = !s->runtime.fullscreen;
        // if (s->runtime.fullscreen)
        //     MaximizeWindow();
        // else 
        //     SetWindowSize(800,600);
        ToggleFullscreen();
    }

    switch (s->runtime.state) {
        // TODO: add a small background simulation?
        case A2_STATE_INTRO:
            if(a2_update_intro(s)) {
                s->runtime.state = A2_STATE_GAMEPLAY;
            }
            break;
        case A2_STATE_GAMEPLAY:

            // TODO: stop all the sound 
            if(IsKeyPressed(KEY_P))
                s->data.paused = !s->data.paused;

            if(!s->data.paused && IsWindowFocused())
                a2_update_gameplay(s);
            break;
        case A2_STATE_GAMEPLAYEND:
            if(a2_update_cleanup(s)) {
                s->runtime.state = A2_STATE_MENU;
                s->data.timers.intro = duration;
            }
            break;

        default:  break;
    }
}

void a2_tick_ui(GameState* s) {
    float duration = s->config.A2C_TIMER_DURATION_INTRO;
    switch(s->runtime.state) {
        
        // TODO: make menu API more obvious
        // NOTE: menu has no break and "falls through"
        // NOTE: comment is to remove most C compilers warning '-W-implicit-fallthrough'
        //
        case A2_STATE_SETTINGS:
            a2_ui_menu_settings(s);
            // fallthrough
        case A2_STATE_MENU:
            // TODO: "slide menu off screen"
        case A2_STATE_GAMEPLAYEND:
        case A2_STATE_INTRO:
            if(a2_ui_menu(s)) {
                s->runtime.state = A2_STATE_INTRO;
                s->data.timers.intro = duration;
            }
            break;
        default: break;
    }

    // for player score specifically
    switch(s->runtime.state) {
        case A2_STATE_INTRO:
        case A2_STATE_GAMEPLAY:
        case A2_STATE_GAMEPLAYEND:
            a2_ui_player_score(s);
            break;
    }
}

#endif//__A2_TICK_H
