#ifndef MEMORY_H
#define MEMORY_H

#include "log.h"
#include "base.h"

void *memory_alloc(usize size);
void memory_release(void* ptr, usize size);

#if defined(MEMORY_STDLIB)
#include <stdlib.h>

void *memory_alloc(usize size) {
    void *ptr = malloc(size);
    ASSERT(ptr != NULL, "malloc error");
    LOG_DBG("malloc(0x%p) allocated 0x%zx bytes", ptr, size);

    return ptr;
}

void memory_release(void *ptr, usize size) {
    if (ptr == NULL || size == 0) {
        return;
    }
    uintptr_t addr = (uintptr_t)ptr; // copy ptr address to avoid use after free warning in log
    free(ptr);
    ptr = NULL;
    LOG_DBG("free(0x%p) released 0x%zx bytes", (void*)addr, size);
}

#elif defined(MEMORY_MMAP)
#include <sys/mman.h>
#include <unistd.h>

static usize _memory_internal_pageSize = 0;

void *memory_alloc(usize size) {
    if (_memory_internal_pageSize == 0) {
        _memory_internal_pageSize = sysconf(_SC_PAGESIZE);
    }

    usize size_aligned = ROUND_DIV(size, _memory_internal_pageSize) * _memory_internal_pageSize;
    void *ptr = mmap(NULL, size_aligned, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT(ptr != MAP_FAILED, "mmap error");
    LOG_DBG("mmap(0x%p) allocated 0x%zx bytes", ptr, size_aligned);

    return ptr;
}

void memory_release(void *ptr, usize size) {
    ASSERT(_memory_internal_pageSize != 0, "release called without alloc");

    if (ptr == NULL || size == 0) {
        return;
    }
    usize size_aligned = ROUND_DIV(size, _memory_internal_pageSize) * _memory_internal_pageSize;
    uintptr_t addr = (uintptr_t)ptr; // copy ptr address to avoid use after free warning in log
    ASSERT(munmap(ptr, size_aligned) == 0, "munmap error");
    LOG_DBG("munmap(0x%p) released 0x%zx bytes", (void*)addr, size_aligned);
}
#else
#error at least one memory backend must be defined
#endif

#endif /* MEMORY_H */
