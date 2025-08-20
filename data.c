
#ifndef __A2_USER_DATA_H
#define __A2_USER_DATA_H



// Definition of game state
// can me modified

enum {
    A2_STATE_MENU, 
    A2_STATE_INTRO, 
    A2_STATE_SETTINGS,
    A2_STATE_GAMEPLAY,
    A2_STATE_GAMEPLAYEND,
};


// Global constants
#include "core/util.c"
typedef struct {
    Color A2C_WINDOW_BACKGROUND_COLOR       ;

    float A2C_ENTITY_PLAYER_ROTATION_SPEED  ;
    float A2C_ENTITY_PLAYER_ACCELERATION    ;
    float A2C_ENTITY_PLAYER_DECELERATION    ;
    float A2C_ENTITY_PLAYER_SCALE           ;
    float A2C_ENTITY_PLAYER_LINE_THICKNESS  ;
    
    Color A2C_ENTITY_PLAYER_ENGINE_COLOR    ;
    Color A2C_ENTITY_PLAYER_TRAIL_COLOR     ;
    Color A2C_ENTITY_BULLET_COLOR           ;
    float A2C_ENTITY_BULLET_LINE_THICKNESS  ;

    float A2C_PHYSICS_VEL_STOP_THRESHOLD    ;
    float A2C_PHYSICS_DRAG_COEF             ;
    
    float A2C_TIMER_DURATION_INTRO;
    
    float A2C_DEBUG_FACE_LING               ;
} Constants;

Constants a2_constants_default(void) {
    return (Constants) {
        .A2C_WINDOW_BACKGROUND_COLOR         = col(20,20,20),
        .A2C_ENTITY_PLAYER_ROTATION_SPEED    =   220,
        .A2C_ENTITY_PLAYER_ACCELERATION      =    20,
        .A2C_ENTITY_PLAYER_DECELERATION      =  12.5,
        .A2C_ENTITY_PLAYER_SCALE             =    10,
        .A2C_ENTITY_PLAYER_LINE_THICKNESS    =  1.75,
        .A2C_ENTITY_PLAYER_ENGINE_COLOR      = col(180,140,240),
        .A2C_ENTITY_PLAYER_TRAIL_COLOR       = col(140,140,240),
        .A2C_ENTITY_BULLET_COLOR             = col(240,230,140),
        .A2C_ENTITY_BULLET_LINE_THICKNESS    =     3,
        .A2C_PHYSICS_VEL_STOP_THRESHOLD      =   0.2,
        .A2C_PHYSICS_DRAG_COEF               =  0.02,
        .A2C_DEBUG_FACE_LING                 =  50.0,
        .A2C_TIMER_DURATION_INTRO            =  1.80,
    };
} 


// Entity data (custom)
typedef struct {
    bool    die;
    int     asteroid_type;
    int     hp;
    int     score;
    Vector2 engine_position;
    float   engine_trust;
    float   hurt_time; 
    float   score_time;
    float   timer1;
    float   gun_overload;
} EntityData;

EntityData a2_entity_data_default(void) {
    return (EntityData) {0}; // zeroed
}


typedef struct {
    float volume;
    Color bullet_color;
    Color engine_color;
    Color trail_color;
    bool  use_rainbow_score;
    bool  enable_flicker;
} GameSettings;

void a2_default_settings(GameSettings* s) {
    assert(s);
    *s = (GameSettings) {
        .volume                 = 0.25,
        .bullet_color           = {240,230,140,255},
        .engine_color           = {180,140,240,255},
        .trail_color            = {200,140,240,255},
        .use_rainbow_score      = true,
        .enable_flicker         = false,
    };
}

typedef struct {
    int best_score;
} GameUserData;

// Global variables
typedef struct {
    bool paused;

    GameUserData user;
    GameSettings settings;

    struct {
        float intro;
    } timers;

} GlobalData;


// hardcoded paths
#if _WIN32    
#   define PATH_CHAR "\\"
#   define CFG_PATH getenv("AppData")
#else
#   define PATH_CHAR "/"
#   define CFG_PATH getenv("HOME")
#endif
#define DATA_DIR_NAME "asteroids2"

#if _WIN32
#   define BASE_PATH() TextFormat("%s%s%s", CFG_PATH, PATH_CHAR, DATA_DIR_NAME)
#else
#   define BASE_PATH() TextFormat("%s%s%s%s%s", CFG_PATH,PATH_CHAR,".config", PATH_CHAR, DATA_DIR_NAME)
#endif

enum {
    A2_FILE_SETTINGS,
    A2_FILE_DATA,
};

enum {
    A2_NONE_OK,
    A2_SETTINGS_OK,
    A2_DATA_OK,
};

// path_data
typedef struct {
    char base_path[512];
    char paths[2] [512];
} PathData;

void a2_get_paths(PathData* p) {
    const char* fmt;
    
    size_t bp_size = strlen(BASE_PATH());
    memcpy(&p->base_path, BASE_PATH(),  min(512, bp_size));


    for(int i = 0; i < 2; i++) {
        const char* file_name;
        switch(i){
            case A2_FILE_SETTINGS: file_name = "settings.bin"; break;
            case A2_FILE_DATA:     file_name = "data.bin";     break;
            default: assert(0);
        }

        fmt = TextFormat("%s%s%s",p->base_path, PATH_CHAR, file_name);
        memcpy(&p->paths[i], fmt,  min(512, strlen(fmt)));
    }
}

void a2_save_files(GameUserData* d, GameSettings* s) {
    PathData p = {0};
    a2_get_paths(&p);
    
    SaveFileData(p.paths[A2_FILE_SETTINGS], s, sizeof(*s));
    SaveFileData(p.paths[A2_FILE_DATA],     d, sizeof(*d));
}

void a2_load_file_data(void* out, size_t size, char* data, int datsize) {
    if ((size_t)datsize != size) {
        TraceLog(LOG_WARNING, "Loaded config and expected config have mismatched sizes! INFO: %lu :: %lu", 
                size, datsize);
        memcpy(out, data, datsize);
    } 
    
    else memcpy(out, data, size);
}

int a2_load_files(GameUserData* d, GameSettings* s) {
    assert(d);
    assert(s);
    int ok = A2_NONE_OK;
 
    PathData p = {0};
    a2_get_paths(&p);

    if(!DirectoryExists(p.base_path)) {
        if(MakeDirectory(p.base_path) != 0) 
            TraceLog(LOG_WARNING, "Failed to create base_path '%s' at %s", DATA_DIR_NAME, p.base_path);
        else 
            TraceLog(LOG_INFO, "Made base_pathectory: %s", p.base_path);
    }

    printf("s1:%s\ns2:%s\n",p.paths[0],p.paths[1]);
    
    for(int i = 0; i < 2; i++) if(!FileExists(p.paths[i])) {
        if(!SaveFileText(p.paths[i], "")); else TraceLog(LOG_ERROR,"failed to create %s", p.paths[i]);
    }

    int sizes[2];
    char* data     = (char*)LoadFileData(p.paths[A2_FILE_DATA],     &sizes[A2_FILE_DATA]);
    char* settings = (char*)LoadFileData(p.paths[A2_FILE_SETTINGS], &sizes[A2_FILE_SETTINGS]);

    if (data) {
        ok |= A2_DATA_OK;
        a2_load_file_data(d,sizeof(*d), data, sizes[A2_FILE_DATA]);
    }
    else 
        TraceLog(LOG_ERROR, "LoadFileText(\"%s\") has failed",  p.paths[0]);

    if (settings) {
        ok |= A2_SETTINGS_OK;
        a2_load_file_data(s,sizeof(*s), settings, sizes[A2_FILE_SETTINGS]);
    }
    else 
        TraceLog(LOG_ERROR, "LoadFileText(\"%s\") has failed",   p.paths[1]);

    UnloadFileText(data);
    UnloadFileText(settings);
    return ok;
}

GlobalData a2_global_data_default(void) {
    return (GlobalData) {0};
}
#endif//__A2_USER_DATA_H
