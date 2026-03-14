#ifndef ARENA_H
#define ARENA_H

#include "util.h"

typedef struct {
    uintptr_t *data;
    usize size;
    usize capacity;
} Arena;

Arena arena_create(usize capacity);
void arena_destroy(Arena *arena);
void *arena_alloc(Arena *arena, usize size);
void arena_reset(Arena *arena);

#ifdef ARENA_IMPLEMENTATION

#include "log.h"
#include "memory.h"
#include "util.h"

Arena arena_create(usize capacity) {
    usize word_size = MEMBER_SIZE(Arena, data);
    usize capacity_aligned = ROUND_DIV(capacity, word_size);
    uintptr_t *data = memory_alloc(capacity_aligned * word_size);
    ASSERT(data != NULL, "not enough memory");

    return (Arena){
        .size = 0,
        .capacity = capacity_aligned,
        .data = data,
    };
}

void arena_destroy(Arena *arena) {
    ASSERT(arena != NULL, "arena is NULL");
    if (arena->data == NULL) {
        return;
    }

    usize word_size = MEMBER_SIZE(Arena, data);
    memory_release(arena->data, arena->capacity * word_size);
    MEMORY_ZERO_STRUCT(arena);
}

void *arena_alloc(Arena *arena, usize size) {
    ASSERT(arena != NULL, "arena is NULL");

    usize word_size = MEMBER_SIZE(Arena, data);
    usize size_aligned = ROUND_DIV(size, word_size);
    ASSERT(arena->size + size_aligned <= arena->capacity,
           "size exceeds capacity");

    void *ptr = &arena->data[arena->size];
    arena->size += size_aligned;
    LOG_DBG("(0x%p) allocated 0x%zx/0x%zx bytes", ptr, size_aligned * word_size,
            arena->capacity * word_size);

    return ptr;
}

void arena_reset(Arena *arena) { arena->size = 0; }
#endif

#endif /* ARENA_H */
