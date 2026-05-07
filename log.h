#ifndef LOG_H
#define LOG_H

#include "base.h"

#define ANSI_RESET "\x1b[0;0m"

#define ANSI_BLACK "\x1b[0;30m"
#define ANSI_RED "\x1b[0;31m"
#define ANSI_GREEN "\x1b[0;32m"
#define ANSI_YELLOW "\x1b[0;33m"
#define ANSI_BLUE "\x1b[0;34m"
#define ANSI_MAGENTA "\x1b[0;35m"
#define ANSI_CYAN "\x1b[0;36m"
#define ANSI_WHITE "\x1b[0;38m"

#define LOG_LEVELS(_) \
    _(DBG, dbg) \
    _(INF, inf) \
    _(WRN, wrn) \
    _(ERR, err) \

#define AS_LOG_LEVEL(lvl, _) LOG_##lvl,
#define AS_LOG_LEVEL_STR(_, str) #str,

typedef enum {
    LOG_LEVELS(AS_LOG_LEVEL)
} LogLevels;

#include <stdio.h>

#define __LOG_COMMON(_level, _color, _msg, ...)                                      \
do {                                                                                 \
    if ((_level) < _log_internal_level) break;                                       \
    static const char* _log_internal_level_str[] = { LOG_LEVELS(AS_LOG_LEVEL_STR) }; \
    Time t = usec_to_time(_log_internal_uptime());                                     \
    fprintf(stderr,                                                                  \
            _color "[" TIME_FORMAT "] <%s> " __FILE__                                \
                ":" STR(__LINE__) ":%s: " _msg ANSI_RESET "\n",                      \
            TIME_EXPAND(&t),                                                         \
            _log_internal_level_str[(_level)],                                       \
            __func__,                                                                \
            ##__VA_ARGS__);                                                          \
} while (0)

#define LOG_DBG(msg, ...) __LOG_COMMON(LOG_DBG, ANSI_BLUE, msg, ##__VA_ARGS__)
#define LOG_INF(msg, ...) __LOG_COMMON(LOG_INF, ANSI_GREEN, msg, ##__VA_ARGS__)
#define LOG_WRN(msg, ...) __LOG_COMMON(LOG_WRN, ANSI_YELLOW, msg, ##__VA_ARGS__)
#define LOG_ERR(msg, ...) __LOG_COMMON(LOG_ERR, ANSI_RED, msg, ##__VA_ARGS__)

void log_init(LogLevels level);

#ifdef LOG_IMPLEMENTATION

#include "os.h"

static u64 _log_internal_startUsec = 0;
static LogLevels _log_internal_level = LOG_DBG;

void log_init(LogLevels level) {
    _log_internal_startUsec = os_now_usec();
    _log_internal_level = level;
}

static inline u64 _log_internal_uptime(void) {
    return os_now_usec() - _log_internal_startUsec;
}

#endif /* LOG_IMPLEMENTATION */

#endif /* LOG_H */
