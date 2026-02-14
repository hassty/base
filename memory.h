#ifndef MEMORY_H
#define MEMORY_H

#include "log.h"
#include "util.h"

#if defined(MEMORY_STDLIB)
#include <stdlib.h>
#include <unistd.h>
static inline void *malloc_aligned(usize size) {
    void *ptr = malloc(size);
    ASSERT(ptr != NULL, "malloc error");
    LOG_DBG("malloc(0x%p) allocated 0x%zx bytes", ptr, size);

    return ptr;
}
static inline void free_aligned(void *ptr, usize size) {
    if (ptr == NULL || size == 0) {
        return;
    }
    free(ptr);
    LOG_DBG("free(0x%p) released 0x%zx bytes", ptr, size);
}

#define MEMORY_ALLOC(size) (malloc_aligned(size))
#define MEMORY_RELEASE(ptr, size) (free_aligned(ptr, size))
#elif defined(MEMORY_MMAP)
#include <sys/mman.h>
#include <unistd.h>

static inline void *mem_alloc(usize size) {
    long pagesize = sysconf(_SC_PAGESIZE);

    usize size_aligned = ROUND_DIV(size, pagesize) * pagesize;
    void *ptr = mmap(NULL, size_aligned, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT(ptr != MAP_FAILED, "mmap error");
    LOG_DBG("mmap(0x%p) allocated 0x%zx bytes", ptr, size_aligned);

    return ptr;
}

static inline void mem_release(void *ptr, usize size) {
    if (ptr == NULL || size == 0) {
        return;
    }
    long pagesize = sysconf(_SC_PAGESIZE);
    usize size_aligned = ROUND_DIV(size, pagesize) * pagesize;
    ASSERT(munmap(ptr, size_aligned) == 0, "munmap error");
    LOG_DBG("munmap(0x%p) released 0x%zx bytes", ptr, size_aligned);
}

#define MEMORY_ALLOC(size) (mem_alloc((size)))
#define MEMORY_RELEASE(ptr, size) (mem_release((ptr), (size)))
#else
#error at least one memory backend must be defined
#endif

#endif /* MEMORY_H */
