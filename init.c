#include "base/base.h"

void a2_entities_init(Pool* ents, Constants cfg) {
    a2_entity_spawn(ents, a2_create_gate(cfg));
    a2_entity_spawn(ents, a2_create_player(cfg));
}

void a2_systems_init(GameState* s) {
    a2_append_system(s, a2_asteroid_system_spawn);
    a2_append_system(s, a2_player_system_ondeath);
}

void a2_shaders_init(GameState* s) {

    Shader blur = LoadShaderFromMemory(0,a2_player_blur_fragment());
    Vector2 fbo_size = {2560,1440}; // HARDCODED
    float str = 2; // blur strength

    // upload values to the GPU
    SetShaderValue(blur, GetShaderLocation(blur, "resolution"), &fbo_size, SHADER_UNIFORM_VEC2);
    SetShaderValue(blur, GetShaderLocation(blur, "strength"),   &str, SHADER_UNIFORM_FLOAT);

    ShaderSystem* shaders = &(s->shaders);
    a2_shader_append_fragment(shaders, blur, a2_player_trail_draw, &s->camera, &s->config,  1);
}
