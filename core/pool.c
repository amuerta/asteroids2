#include "types.h"
#include "entity.c"

#ifndef __POOL_H
#define __POOL_H

#define INDEX_INVALID ((size_t)-1)

Entity* a2_entpool_refer(Pool* p, index_t ptr) {
    // Pool MUST BE A POINTER
	//static Entity* tmp = NULL; 
	//tmp = &(p.buffer[ptr]);
	return (p->state_buffer[ptr]) ? &(p->buffer[ptr].as_entity) : NULL;
}

index_t 	a2_entpool_reserve(Pool* p) {
	index_t ptr = INDEX_INVALID;
	if (p->free_count > 0) {
		ptr = p->free_buffer[p->free_count-1];
		p->free_count--;
	} else {
		ptr = p->count;
	}
	assert(ptr != INDEX_INVALID && "Failed to reserve entity");
	
	p->state_buffer		[ptr] |= POOL_ALLOCATED;
	assert(p->count < MAX_ENTITY_COUNT);

	p->count++;
	if (p->count > p->max_size) {
		p->max_size = p->count;
	}
	return ptr;
}

void	a2_entpool_release(Pool* p, index_t ptr) {
	if (p->count==0)
		return;

	assert(ptr < MAX_ENTITY_COUNT && "Attempt to access Out of Bounds");

	if (!(p->state_buffer[ptr] & POOL_ALLOCATED))
		return;
	

	p->free_buffer[p->free_count] = ptr;
	p->free_count++;

	p->state_buffer[ptr] ^= POOL_ALLOCATED;
	p->count--;
}

//
// Entity creation / destruction
//

void a2_entity_spawn(Pool* ents, Entity template)  {
    index_t id = a2_entpool_reserve(ents);
    Entity* e  = a2_entpool_refer(ents, id);
    assert(e);
    Entities collision_list = e->collided;

    memcpy(e, &template, sizeof(Entity));
    // carry over the allocated buffer (like collided buffer, id)
    e->collided = collision_list;
    e->collided.count = 0;
    e->id = id;
}

// TODO: query entites for despawn, swipe them at the end of the frame.
void a2_entity_despawn(Pool* ents, Entity* ent) {
    assert(ent && ents);
    index_t id = ent->id;
    //memset(ent,0 ,sizeof(*ent));
    a2_entpool_release(ents, id);
}

#endif //__GAME_POOL_H
