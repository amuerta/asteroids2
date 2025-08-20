#include "game.c"

// GAME REQUIREMENTS:
//
// + Asteroids that spawn off screen
// + Asteroids off screen
// + Player
//  - there are 2 player selected ships: gunship and lazership
// 	- move
// 	- shoot
// 	- gun has recoil but bigger damage
// 	- lazer has no recoil but is less effective
// 	- make player not get lost (wrap around screen edges)
// 	? player has speed break on cooldown
// 	- missle that homes in a little and deals AOE
// 	- railgun that instabreaks all asteroids in straight line
// 	- background shader 
// 	- shader and/or particles for bullet, explosions, debrie and engine
// + GUI (Graphical User Interface)
//  - score counter in the middle of the screen
// 	- start game screen
// 	- loose screen
// 	- statistics
// 	- settings or something


// TODO: configurable ui
// TODO: options for player trail:
//          decay, width_scaling
// TODO: save progress in file into system specific locations
// TODO: make sure no crashes

int main(void) {
    GameState a2 = {
        .runtime = {
            .target_fps = 60,
        }
    };

    // window 
    SetConfigFlags(
            FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT |
            FLAG_WINDOW_RESIZABLE
    );

    SetTargetFPS(a2.runtime.target_fps);
    InitWindow(800,600,"Asteroids 2");
#ifndef DEBUG
    SetExitKey(KEY_F4);
#endif
    
    // audio
    InitAudioDevice();
    SetMasterVolume(0.25);

    int load_result = a2_load_files(&a2.data.user, &a2.data.settings);
    if(!(load_result & A2_SETTINGS_OK)) 
        a2_default_settings(&a2.data.settings);

    a2_initilize(&a2);
    
    while(!WindowShouldClose()) {
        if (a2.runtime.terminate) break;
        if (a2.runtime.refresh || a2.runtime.tick == 0) 
            a2_refresh(&a2);
        a2_update(&a2);
        a2_draw(&a2);
    }
    
    a2_save_files(&a2.data.user, &a2.data.settings);
    a2_deinitilize(&a2);

    CloseAudioDevice();
    CloseWindow();
}
