#include "definitions.h"

#ifndef __A2_BASE_DEBUG_H
#define __A2_BASE_DEBUG_H

// Debug
void a2_base_draw_debug_window(GameState* s) {
    mu_Context* ctx = &(s->uictx);
    int flags =   MU_OPT_NOCLOSE | MU_OPT_ALIGNCENTER;
    Vector2 win_size = {s->arena[0].width, s->arena[0].height};
    Entity* player = 0;

    size_t entity_count = s->entities.count;
    size_t 
        aster_count  = 0,
        bullet_count = 0
    ;

    for(size_t i = 0; i < s->entities.max_size; i++)  {
        if (!s->entities.state_buffer[i]) continue;
        switch(s->entities.buffer[i].as_entity.type) {
            case A2_ENTITY_TYPE_ASTEROID:
                aster_count++; 
                break;
            case A2_ENTITY_TYPE_BULLET:
                bullet_count++; 
                break;
            case A2_ENTITY_TYPE_PLAYER:
                player = &s->entities.buffer[i].as_entity;
                break;
        }
    }

    int size = 200;
    mu_begin_window_ex(ctx, "Debug", mu_rect(win_size.x-size,0,size,size*2),flags);
    {
        mu_layout_row(ctx, 1, (int[]) { -1 }, 0);

        mu_label(ctx, "Key for debug info: Shift +'/'");

        if (mu_header(ctx, "Runtime")) {
            mu_label(ctx, TextFormat("refresh: %s", bool_fmt(s->runtime.refresh)    ));
            mu_label(ctx, TextFormat("resize : %s", bool_fmt(s->runtime.resize)     ));
        }

        if (mu_header(ctx, "Global Data")) {
            mu_label(ctx, "timers:");
            mu_label(ctx, TextFormat("intro: %f", s->data.timers.intro));
        }

        if (mu_header(ctx, "Frame")) {
            mu_label(ctx, TextFormat("tick: %li", s->runtime.tick));
            mu_label(ctx, TextFormat("target fps: %li", s->runtime.target_fps));
            mu_label(ctx, TextFormat("current fps: %i", GetFPS()));
            mu_label(ctx, TextFormat("frame time: %f", GetFrameTime()));
        }
        if (mu_header(ctx, "Entites")) {
            mu_label(ctx, TextFormat("total count: %lu", entity_count));
            mu_label(ctx, TextFormat("asteroid count: %lu", aster_count));
            mu_label(ctx, TextFormat("bullet count: %lu", bullet_count));
        }
        if (mu_header(ctx, "Player")) {
            // TODO: make player data store names
            // display them here
            mu_layout_height(ctx, 100);
            mu_begin_panel(ctx,"player_pane");
            if(player) {
                if(mu_header(ctx, "data")) {
                    mu_layout_height(ctx, 100);
                    mu_begin_panel(ctx,"player_data");
                    mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
                    mu_label(ctx, TextFormat("overload: %.2f", player->data.gun_overload));
                    mu_end_panel(ctx);
                }
                if(mu_header(ctx, "model")) {
                    mu_layout_height(ctx, 100);
                    mu_begin_panel(ctx,"player_model");
                    for(size_t i = 0; i < player->base.count; i++) {
                        mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
                        mu_label(ctx,TextFormat("#%-10i [ %1.f, %1.f ]", 
                                    i,
                                    player->base.poly[i].x,
                                    player->base.poly[i].y
                        ));
                    }
                    mu_end_panel(ctx);
                }
            } else mu_label(ctx, "player: failed to find");

            mu_end_panel(ctx);
        }
    }
    mu_end_window(ctx);
}

#endif//__A2_BASE_DEBUG_H
