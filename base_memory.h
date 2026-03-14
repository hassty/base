#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

#include "util.h"

typedef void *m_reserve_func(void *ctx, u64 size);
typedef void m_change_memory_func(void *ctx, void *ptr, u64 size);

typedef struct {
    m_reserve_func *reserve;
    m_change_memory_func *commit;
    m_change_memory_func *decommit;
    m_change_memory_func *release;
    void *ctx;
} m_base_memory;

static inline void m_change_memory_func_noop(void *ctx, void *ptr, u64 size) {
    UNUSED(ctx);
    UNUSED(ptr);
    UNUSED(size);
}

typedef struct {
    m_base_memory *base;
    u8 *memory;
    u64 capacity;
    u64 position;
    u64 commit_position;
} m_arena;

typedef struct {
    m_arena *arena;
    u64 position;
} m_temp;

#define M_DEFAULT_RESERVE_SIZE GB(1)
#define M_COMMIT_BLOCK_SIZE MB(64)

m_arena m_make_arena_reserve(m_base_memory *base, u64 reserve_size);
m_arena m_make_arena(m_base_memory *base);
void m_arena_release(m_arena *arena);
void *m_arena_push(m_arena *arena, u64 size);
void *m_arena_push_zero(m_arena *arena, u64 size);
void m_arena_pop_to(m_arena *arena, u64 position);
void m_arena_pop_amount(m_arena *arena, u64 amount);
void m_arena_align(m_arena *arena, u64 pow2_align);
void m_arena_align_zero(m_arena *arena, u64 pow2_align);

#define PUSH_ARRAY(arena, type, count)                                         \
    (type *)m_arena_push((arena), sizeof(type) * (count))
#define PUSH_ARRAY_ZERO(arena, type, count)                                    \
    (type *)m_arena_push_zero((arena), sizeof(type) * (count))

m_temp m_begin_temp(m_arena *arena);
void m_end_temp(m_temp temp);

#endif /* BASE_MEMORY_H */
