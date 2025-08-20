#include "types.h"

#ifndef __A2_ENTITY_DATA
#define __A2_ENTITY_DATA

typedef long long int Slot; // platform independent 
                                  // 8 bytes of data for 
                                  // any value that could be 
                                  // fited in, like:
                                  // - Clamped Rectangle (ints)
                                  // - Vector2,
                                  // - Color,
                                  // - Pointer,
                                  // - int, float, long, double
                                  // - bitmask

typedef struct {
    Slot    *items;
    size_t  count, capacity;
} DataSlots;


// getters
int a2_entdata_get_int(DataSlots d, int i) {
    assert(i < d.count);
    if (!d.items) return 0;
    int it = *(int*)&d.items[i];
    return it;
}

float a2_entdata_get_float(DataSlots d, int i) {
    assert(i < d.count);
    if (!d.items) return 0;
    float it = *(float*)&d.items[i];
    return it;
}

Vector2 a2_entdata_get_vec2(DataSlots d, int i) {
    assert(i < d.count);
    if (!d.items) return (Vector2){0};
    Vector2 it = *(Vector2*)&d.items[i];
    return it;
}

Color a2_entdata_get_color(DataSlots d, int i) {
    assert(i < d.count);
    if (!d.items) return (Color){0};
    Color it = *(Color*)&d.items[i];
    return it;
}

// referencer-(s?)

Vector2* a2_entdata_ref_vec2(DataSlots* d, int i) {
    assert(i < d->count);
    if (!d->items) return NULL;
    // DataSlots has to be a pointer
    Vector2* it = (Vector2*) (d->items + i);
    return it;
}


int* a2_entdata_ref_int(DataSlots* d, int i) {
    assert(i < d->count);
    if (!d->items) return NULL;
    // DataSlots has to be a pointer
    int* it = (int*) (d->items + i);
    return it;
}

#endif//__A2_ENTITY_DATA
