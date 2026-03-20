#ifndef OS_H
#define OS_H

#include "base.h"

u64 OsNowUsec(void);
void OsSleepUsec(u64 sleepUsec);

#ifdef OS_IMPLEMENTATION_LINUX

#include <time.h>


u64 OsNowUsec(void) {
    struct timespec now = {0};
    clock_gettime(CLOCK_MONOTONIC, &now);
    return SEC_TO_USEC(now.tv_sec) + NSEC_TO_USEC(now.tv_nsec);
}

void OsSleepUsec(u64 sleepUsec) {
    u64 sleepSec = USEC_TO_SEC(sleepUsec);
    u64 sleepNsec = USEC_TO_NSEC(sleepUsec - SEC_TO_USEC(sleepSec));
    struct timespec req = {
        .tv_sec = sleepSec,
        .tv_nsec = sleepNsec,
    };
    while (nanosleep(&req, &req) == -1)
        continue;
}

#endif /* OS_IMPLEMENTATION_LINUX */

#endif /* OS_H */
