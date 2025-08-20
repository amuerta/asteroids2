#include "depend.h"
#include "types.h"

#ifndef __A2_SHADERS_H
#define __A2_SHADERS_H

#define TEMP_FBO_COUNT 16

struct ShaderDefinition;
typedef void (*FnShaderSystem) (struct ShaderDefinition* shader, void* pool, Constants* v);

typedef struct ShaderDefinition {
    Vector2         screen;
    Camera2D        *cam;
    Constants*      vars; 
    Shader          sd;
    FnShaderSystem  run;
    RenderTexture   items[TEMP_FBO_COUNT]; // temporary render texture
    size_t count, capacity;
} ShaderDefinition;

typedef struct {
    ShaderDefinition* items;
    size_t count, capacity;
} ShaderSystem;

void a2_shader_append_fragment(
        ShaderSystem* ss, 
        Shader sd,
        FnShaderSystem run, 
        Camera2D* cam,
        Constants* vars,
        size_t fbos
        )

{
    ShaderDefinition s = {
        .screen = {2560,1440}, // hardcoded
        .sd = sd,
        .vars = vars,
        .run = run,
        .capacity = TEMP_FBO_COUNT,
        .count  = fbos,
        .cam = cam
    };
    assert(fbos < TEMP_FBO_COUNT);
    assert(IsShaderValid(s.sd));
    for(size_t i = 0; i < fbos; i++)
        s.items[i] = LoadRenderTexture(2560,1440); // hard code the texture size
    da_append(ss, s);
}

#endif//__A2_SHADERS_H
