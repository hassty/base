#include "base_memory.h"
#include "util.h"
#include <stdlib.h>

static void *m_malloc_reserve(void *ctx, u64 size) {
    ARG_UNUSED(ctx);
    return malloc(size);
}

static void m_malloc_release(void *ctx, void *ptr, u64 size) {
    ARG_UNUSED(ctx);
    ARG_UNUSED(size);
    free(ptr);
}

static m_base_memory *m_malloc_base_memory(void) {
    static m_base_memory memory = {0};
    if (memory.reserve == 0) {
        memory.reserve = m_malloc_reserve;
        memory.commit = m_change_memory_func_noop;
        memory.decommit = m_change_memory_func_noop;
        memory.release = m_malloc_release;
    }
    return &memory;
}

m_arena m_make_arena_reserve(m_base_memory *base, u64 reserve_size) {
    m_arena arena = {0};
    arena.base = base;
    arena.memory = base->reserve(base->ctx, reserve_size);
    arena.capacity = reserve_size;
    return arena;
}

m_arena m_make_arena(m_base_memory *base) {
    m_arena arena = m_make_arena_reserve(base, M_DEFAULT_RESERVE_SIZE);
    return arena;
}

void m_arena_release(m_arena *arena) {
    m_base_memory *base = arena->base;
    base->release(base->ctx, arena->memory, arena->capacity);
}

void *m_arena_push(m_arena *arena, u64 size) {
    if (arena->position + size > arena->capacity) {
        return NULL;
    }

    void *result = arena->memory + arena->position;
    arena->position += size;

    u64 p = arena->position;
    u64 commit_p = arena->commit_position;
    if (p > commit_p) {
        u64 p_aligned = ALIGN_UP_POW2(p, M_COMMIT_BLOCK_SIZE);
        u64 next_commit_p = CLAMP_TOP(p_aligned, arena->capacity);
        u64 commit_size = next_commit_p - commit_p;

        m_base_memory *base = arena->base;
        base->commit(base->ctx, arena->memory + commit_p, commit_size);

        arena->commit_position = next_commit_p;
    }

    return result;
}

void m_arena_pop_to(m_arena *arena, u64 position) {
    if (position >= arena->position) {
        return;
    }

    arena->position = position;

    u64 p = arena->position;
    u64 p_aligned = ALIGN_UP_POW2(p, M_COMMIT_BLOCK_SIZE);
    u64 next_commit_p = CLAMP_TOP(p_aligned, arena->capacity);

    u64 commit_p = arena->commit_position;
    if (next_commit_p < commit_p) {
        u64 decommit_size = commit_p - next_commit_p;
        m_base_memory *base = arena->base;
        base->decommit(base->ctx, arena->memory + next_commit_p, decommit_size);
        arena->commit_position = next_commit_p;
    }
}

void *m_arena_push_zero(m_arena *arena, u64 size) {
    void *result = m_arena_push(arena, size);
    MEMORY_ZERO(result, size);
    return result;
}

void m_arena_align(m_arena *arena, u64 pow2_align) {
    u64 p = arena->position;
    u64 p_aligned = ALIGN_UP_POW2(p, pow2_align);
    u64 size = p_aligned - p;
    if (size > 0) {
        m_arena_push(arena, size);
    }
}

void m_arena_align_zero(m_arena *arena, u64 pow2_align) {
    u64 p = arena->position;
    u64 p_aligned = ALIGN_UP_POW2(p, pow2_align);
    u64 size = p_aligned - p;
    if (size > 0) {
        m_arena_push_zero(arena, size);
    }
}

m_temp m_begin_temp(m_arena *arena) {
    return (m_temp){
        .arena = arena,
        .position = arena->position,
    };
}

void m_end_temp(m_temp temp) { m_arena_pop_to(temp.arena, temp.position); }
