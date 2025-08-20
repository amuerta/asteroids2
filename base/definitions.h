#include "../core/core.h"

#ifndef __A2_DATA_DEFINITIONS_H
#define __A2_DATA_DEFINITIONS_H


typedef enum {
    A2_ENTITY_TYPE_UNDEFINED,
    A2_ENTITY_TYPE_PLAYER,
    A2_ENTITY_TYPE_GATE,
    A2_ENTITY_TYPE_ASTEROID,
    A2_ENTITY_TYPE_BULLET,
    A2_ENTITY_TYPE_RAY,
} EntityType;

typedef enum {
    A2_MODEL_DATA_GUN1,
    A2_MODEL_DATA_ENGINE_START,
    A2_MODEL_DATA_ENGINE_END,
} PlayerModelData;

typedef enum {
    A2_ASTEROID_SMALL,
    A2_ASTEROID_MEDIUM,
    A2_ASTEROID_BIG,
} AsteroidSizeType;

#endif//__A2_DATA_DEFINITIONS_H
