#ifndef OS_H
#define OS_H

#include "base.h"

u64 os_monotonic_usec(void);
void os_sleep_msec(u64 sleep_msec);

#ifdef OS_IMPLEMENTATION_LINUX
#include <time.h>

u64 os_monotonic_usec(void) {
    struct timespec now;
    MEMORY_ZERO_STRUCT(&now);
    clock_gettime(CLOCK_MONOTONIC, &now);
    return SEC_TO_USEC(now.tv_sec) + NSEC_TO_USEC(now.tv_nsec);
}

void os_sleep_msec(u64 sleep_msec) {
    u64 sleepSec = MSEC_TO_SEC(sleep_msec);
    u64 sleepNsec = MSEC_TO_NSEC(sleep_msec - SEC_TO_MSEC(sleepSec));
    struct timespec req;
    req.tv_sec = (time_t)sleepSec;
    req.tv_nsec = (time_t)sleepNsec;
    while (nanosleep(&req, &req) == -1)
        continue;
}
#endif /* OS_IMPLEMENTATION_LINUX */

#ifdef OS_IMPLEMENTATION_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

u64 os_monotonic_usec(void) {
    static LARGE_INTEGER freq;
    if (freq.QuadPart == 0) {
        QueryPerformanceFrequency(&freq);
    }

    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return SEC_TO_USEC(ticks.QuadPart) / freq.QuadPart;
}

void os_sleep_msec(u64 sleep_msec) {
    Sleep((DWORD)sleep_msec);
}
#endif

#endif /* OS_H */
