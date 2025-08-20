#include "depend.h"
#include "sound.c"

#ifndef __A2_TYPES_H
#define __A2_TYPES_H

//
// goodies
//

#include "../lib/hch.h"

//
// defines
//

#define A2_UNUSED(V) ((void) (V))


//
// common types used through out of all the core modules
//

typedef             char    byte;
typedef unsigned    char    ubyte;
typedef             size_t  index_t;
typedef unsigned    char    bitmask8;
typedef unsigned    int     bitmask32;

//
// predelcared typdefs and structs 
// to avoid recursion related issues
//

// good amount
#define MAX_ENTITY_COUNT    512 
#define MAX_SLOTS           64 // up to N values stored in each entity


//
// Entity update/draw "system"
//


typedef void (*UpdateFn)  (
        void* entity, 
        void* pool, 
        void* arena,
        void* config,
        void* sound,
        void* assets
);
typedef void (*DrawFn) (void* entity, void* config);

// depends on model, entity data
#include "model.c"
#include "../data.c"

// c is ... a language ...
struct Entity;
struct Entites; 

typedef struct {
    struct Entity* *items;
    size_t count,  capacity;
} Entities;

typedef struct Entity {
    int         type;
    size_t      id;

    // core data
    Vector2     position;
    Vector2     velocity;
    Vector2     acceleration;
    Vector2     face;
    float       angular_velocity;
    float       rotation;
    float       mass;

    // Collision list

    Entities collided;
    /*
       struct Entity* next;
       struct Entity* tail;
    */

    // Custom model is allowed
    EntityModel       base;

    // specific data - user data
    EntityData        data;

    // default traits that can be run without draw() or update()
    bitmask32   traits;   

    // user definted update and draw PER entity
    DrawFn      draw;
    UpdateFn    update;

    // cache is just something we can avoid computing for some amount of time
    struct {
        EntityModel   rotated;
    }           cache;
} Entity;


// Particles

typedef struct Particle {
    float               lifetime;
    int                 type;
    Color               fg;
    Color               bg;
    Vector2             emitter;
    Vector2             buffer  [32];
    struct Particle*    next;
} Particle;

//
// POOL
//

typedef enum {
    POOL_ALLOCATED = 1,
} PoolState;

typedef union {
    Entity      as_entity;
    Particle    as_particle;
} PoolTypes;

typedef struct {
	PoolTypes	buffer 			[MAX_ENTITY_COUNT];
	bitmask8	state_buffer	[MAX_ENTITY_COUNT];
	index_t		free_buffer		[MAX_ENTITY_COUNT];
	//index_t		reserved_buffer	[MAX_ENTITY_COUNT];
	size_t 		count;
	size_t		free_count;
	size_t		max_size;
} Pool;


//
// game systems - procedure that runs with full GameState information
// to perform some gameplay function related to: 
//  entites, UI, global variable, etc.
//

typedef void (*SystemFn) (void* game_state);

typedef struct {
    SystemFn    *items;
    size_t      count;   
    size_t      capacity;
} Systems;


#endif//__A2_TYPES_H
