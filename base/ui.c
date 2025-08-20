#ifndef __A2_BASE_UI_H
#define __A2_BASE_UI_H

#include "../data.c"
#include "definitions.h"

// Game menu
// TODO: make sure windows are always within screen
// extract this into ui.c
bool a2_ui_menu(GameState *s) {
    mu_Context* ctx = &(s->uictx);

    enum { 
        BUTTON_PLAY, 
        BUTTON_SETTINGS, 
        BUTTON_EXIT 
    };
    const char* labels[] = { 
        "play",     
        "settings", 
        "exit"
    };

    int label_count = (int)ARRAY_LEN(labels);
    int option = -1;
    int button_size = 30;
    int pad = (s->arena[0].height - button_size*label_count)/2;
    mu_Container* settings = 0;
    mu_Container* menu = mu_get_container(ctx, "Asteroids");
    Rectangle screen = s->arena[0];

    // menu has to be of screen height
    if (menu) menu->rect.h = screen.height;

    if(mu_begin_window_ex(ctx, "Asteroids", mu_rect(0,0,120,screen.height), 
                MU_OPT_NOCLOSE | MU_OPT_ALIGNCENTER | MU_OPT_NOINTERACT
        )) 
    {
        
        // dump, but working way of doing padding
        mu_layout_row(ctx, 1, (int[]) { -1 }, pad);
        mu_label(ctx,"");


        mu_layout_row(ctx, 1, (int[]) { -1 }, button_size);
        // TODO: make some actual score display
        mu_header_ex(ctx, TextFormat("Your best: %i", s->data.user.best_score), 
                MU_OPT_ALIGNCENTER | MU_OPT_HIDE_ICON);

        for(int i = 0; i < label_count; i++) {
            bool button_pressed = 
                mu_button(ctx, TextFormat("%s [%i]",labels[i], i+1)) 
                || IsKeyPressed('1'+i);
            if(button_pressed) option = i;
        }
        mu_end_window(ctx);
    } 

    // ignore requests to trigger buttons
    // when game is not ready
    bool menuidle = 
        s->runtime.state == A2_STATE_MENU ||
        s->runtime.state == A2_STATE_SETTINGS
    ;

    // handle button events
    switch(option) {
        case BUTTON_PLAY:
            if (menuidle) return true;
            break;

        case BUTTON_SETTINGS:
            settings = mu_get_container(ctx, "Settings");
            if (settings) settings->open = 1;
            if (menuidle) s->runtime.state = A2_STATE_SETTINGS;
            break;

        case BUTTON_EXIT:
            s->runtime.terminate = true;
            break;

        default: break;
    }

    return false;
}


float ease_in_out_circ(float n) {
    return n < 0.5
        ? (1 - sqrt(1 - pow(2 * n, 2))) / 2
        : (sqrt(1 - pow(-2 * n + 2, 2)) + 1) / 2
    ;
}

void a2_ui_player_score(GameState* s) {

    static Entity* player = 0;
    Pool* pool = &s->entities;

    // select player
    if(!player) for(size_t i = 0; i < pool->max_size; i++) {
        Entity* it = a2_entpool_refer(pool, i);
        if (it && it->type == A2_ENTITY_TYPE_PLAYER) 
            player = it;
    }

    assert(player);

    Rectangle screen = s->arena[ARENA_VISIBLE];
    Vector2 score_box_size = {150,30};
    float timer = s->data.timers.intro;
    float duration = s->config.A2C_TIMER_DURATION_INTRO;
    float font_size = 24;

    // calcualted from timer
    float hide_offset = -score_box_size.y;
    float frac = ease_in_out_circ(Clamp(duration-timer, 0, 1));
    float ypos = hide_offset + frac * score_box_size.y;

    Rectangle pane = {
        (screen.width - score_box_size.x)/2,
        ypos,
        score_box_size.x,
        score_box_size.y,
    };

    Color base = GetColor(0x505050FF);
    bool        flicker = (s->runtime.tick % (s->runtime.target_fps/8));
    const char* score;
    Color       color;

    Color score_color;

    if (s->data.settings.use_rainbow_score)
        score_color = ColorFromHSV(GetTime()*30,0.5,0.75);
    else 
        score_color = player->base.fg;

    switch(s->runtime.state ) {
        case A2_STATE_GAMEPLAY: 
            score = TextFormat("%d", player->data.score);
            color = ColorLerp(base, 
                score_color, 
                player->data.score_time);
            break;
        case A2_STATE_GAMEPLAYEND: 
            score = "signal lost";
            
            if (s->data.settings.enable_flicker) 
                flicker = true; // makes it be 1 which never triggers flicker below
            
            color = ColorLerp(DARKGRAY,col(240,170,130),
                    Clamp((duration-timer) - timer,0,1.0)*flicker);
            break;
        case A2_STATE_INTRO: 
            score = "warping...";
            color = ColorLerp(base, 
                    col(140,230,150), 
                    s->data.timers.intro);
            break;
    }
    

    DrawRectangleRec(pane, GetColor(0x171717FF));
    DrawRectangleLinesEx(pane, 1, GetColor(0x505050FF));
    DrawText(score, 
            pane.x + (pane.width - MeasureText(score, font_size))/2,
            pane.y + (pane.height - font_size)/2,
            font_size,
            color
    );
}

static int mu_uint8_slider(mu_Context *ctx, unsigned char *value, int low, int high) {
  static float tmp;
  mu_push_id(ctx, &value, sizeof(value));
  tmp = *value;
  int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.0f", MU_OPT_ALIGNCENTER);
  *value = tmp;
  mu_pop_id(ctx);
  return res;
}

static void mu_color_slider(mu_Context* ctx, const char* label, Color* c) {
    assert(c);
    const int label_w = 120;
    int remainder = mu_get_current_container(ctx)->body.w - (label_w+24);
    int slider_w = remainder * 0.25;
    mu_layout_row(ctx, 5, (int[]) {label_w, slider_w, slider_w, slider_w, slider_w - 2}, 24);
    mu_label        (ctx, label);
    mu_uint8_slider (ctx, &(c->r), 0, 255);
    mu_uint8_slider (ctx, &(c->g), 0, 255);
    mu_uint8_slider (ctx, &(c->b), 0, 255);
    mu_draw_rect    (ctx, mu_layout_next(ctx), *(mu_Color*)c );
}

static void mu_normalized_slider(mu_Context* ctx, const char* label, float* value) {
    mu_layout_row(ctx, 2, (int[]) { 120, -1 }, 24);
    mu_label(ctx, label);
    mu_slider(ctx, value, 0, 1.0);
}

static void mu_toggle(mu_Context* ctx, const char* label, bool* toggle) {
    mu_layout_row(ctx, 2, (int[]) { 120, -1 }, 24);
    mu_label(ctx, TextFormat("state: [ %5s ]", (*toggle) ? "true" : "false" ));

    // micro ui uses freaking hashmap on labels to know what button to update
    if (mu_button(ctx, TextToLower(label))) 
        *toggle = !(*toggle);
}

// Settings menu
static void a2_ui_menu_settings(GameState* s) {
    float* volume       = &s->data.settings.volume;
    Color* bullet_color = &s->data.settings.bullet_color;     
    Color* engine_color = &s->data.settings.engine_color;     
    Color* trail_color  = &s->data.settings.trail_color;     
    bool*  rainbow      = &s->data.settings.use_rainbow_score;
    bool*  stopflicker  = &s->data.settings.enable_flicker;   

    mu_Context* ctx = &(s->uictx);
    Rectangle screen = s->arena[0];
    Vector2 winsize = {300, 400};
    mu_Rect window_rec = {
        .x = (screen.width - winsize.x) / 2,
        .y = 100,
        .w = winsize.x,
        .h = winsize.y
    };

    if(mu_begin_window_ex(ctx, "Settings", window_rec, MU_OPT_ALIGNCENTER)) {

        mu_normalized_slider(ctx,"Master volume",   volume);
        mu_color_slider(ctx, "Bullet color",        bullet_color);
        mu_color_slider(ctx, "Engine color",        engine_color);
        mu_color_slider(ctx, "Trail color",         trail_color);
        mu_toggle(ctx, "Rainbow score",             rainbow);
        mu_toggle(ctx, "Disable flickering",        stopflicker);

        s->runtime.refresh = true;
        mu_end_window(ctx);
    } 

}

#endif//__A2_BASE_UI_H
