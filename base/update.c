#include "definitions.h"

#ifndef __A2_BASE_UPDATE_H
#define __A2_BASE_UPDATE_H

void a2_update_gameplay(GameState* s) {
    // aliasing
    Constants c        = s->config;
    Pool* ents      = &(s->entities);
    Entity* ent     = 0;

    // run collision system
    a2_system_up_collision(s);

    // run appended gameplay systems
    for(size_t i = 0; i < s->systems.count; i++)
        if(s->systems.items[i]) s->systems.items[i](s);
  
    // update all entity logic
    assert(ents);
    for(size_t i = 0; i < ents->max_size; i++) {
        if ((ent = a2_entpool_refer(ents, i))) 
            a2_entity_update(
                    ent, 
                    ents , 
                    s->arena, 
                    &c, 
                    &(s->sounds),
                    &(s->assets)
            );
    }
}



bool a2_update_intro(GameState* s) {
    mu_Context* ctx = &(s->uictx);
    float timer = s->data.timers.intro;
    float duration = s->config.A2C_TIMER_DURATION_INTRO;
    float swipe = 0;
    Entity* player = 0;
    Entity* gate   = 0;
    mu_Container* menu = mu_get_container(ctx, "Asteroids");

    for(size_t i = 0; i < s->entities.max_size; i++) {
        if (s->entities.state_buffer[i]) {
            switch(s->entities.buffer[i].as_entity.type ) {
                case A2_ENTITY_TYPE_PLAYER: 
                    player = &s->entities.buffer[i].as_entity ;
                    break;

                case A2_ENTITY_TYPE_GATE: 
                    gate = &s->entities.buffer[i].as_entity;
                    break;
            }
        }
    }

    // must exist
    assert(player);
    assert(gate);
    
    // timer is not over
    if (timer >= 0) {
        
        // begin updating player and game
        if (timer < duration/2) {
            player->base.hide = true;
            a2_update_gameplay(s);
        } else  {
            // calculate swipe for menu and gate
            swipe = Clamp(duration-timer, 0, 1);
            gate->data.timer1 = Clamp(duration-timer, 0, 1);

            if (menu) {
                menu->rect.x = -(menu->rect.w*ease_out_quint(swipe)) ;
            }

        }
        if (float_approx_eq(timer, duration/2, 0.01)) 
            player->acceleration = vec2(0,-300);
        
        if (float_approx_eq(timer, duration/2+0.25, 0.01)) {
            a2_playsound(&s->sounds,s->assets.warp);
            player->base.hide = false;
        }

        if (timer < duration/2+0.25)
            player->base.hide = false;

        if (timer < duration/2) 
            gate->data.timer1 = timer;


    } 

    // finished
    else {
        gate->data.timer1 = 0;
        return true;
    }


    s->data.timers.intro -= GetFrameTime();
    return false;
}

// TODO: add some sort of wait timer for particles
// TODO: make reset_player seprate function??
bool a2_update_cleanup(GameState* s) {

    mu_Context* ctx = &(s->uictx);
    float duration = s->config.A2C_TIMER_DURATION_INTRO;
    float timer = s->data.timers.intro;
    float swipe = 0;
    mu_Container* menu = mu_get_container(ctx, "Asteroids");

    size_t  asteroid_count = 0;
    Constants c        = s->config;
    Pool* ents      = &(s->entities);
    Entity* it;
    Entity* player;

    // TODO: use a2_pool_refer
    for(size_t i = 0; i < s->entities.max_size; i++) {
        if (s->entities.state_buffer[i]) {
            switch( (it = &s->entities.buffer[i].as_entity)->type ) {
                case A2_ENTITY_TYPE_PLAYER:
                    player = it;
                    break;
                case A2_ENTITY_TYPE_ASTEROID: 
                    asteroid_count++;
                    // fall through
                case A2_ENTITY_TYPE_BULLET:
                    it->acceleration = vec2(0,-25); 
                    it->face = vec2(0,-10);
                    it->rotation *= 0.001; 
                    a2_entity_update(
                            it, 
                            ents , 
                            s->arena, 
                            &c, 
                            &(s->sounds),
                            &(s->assets)
                            );
                    break;

            }
        }
    }


    // reset player and player data
    // set best score
    if(!asteroid_count) {
        int prev_score = s->data.user.best_score;
        s->data.user.best_score = max(player->data.score, prev_score);

        player->data.die = false; 
        player->data.score_time = 0; 
        player->data.score = 0; 
        player->position = vec2(0,0);
        player->rotation = 0;
        player->velocity = Vector2Zero();
        player->acceleration = Vector2Zero();
        player->face  = vec2(0,-1);
        player->collided.count = 0;
        player->cache.rotated = (EntityModel){0};
    }

    if(menu) { 
        menu->open = 1; 
        swipe = Clamp(timer, 0, 1);
        menu->rect.x = -menu->rect.w + menu->rect.w * ease_out_quint(swipe);
    }

    if (timer < duration) {
        s->data.timers.intro += GetFrameTime();
    }

 


    
    return !asteroid_count && timer >= duration;
}



#endif//__A2_BASE_UPDATE_H
