#ifndef LOG_H
#define LOG_H

#define ANSI_RESET "\x1b[0;0m"

#define ANSI_BLACK "\x1b[0;30m"
#define ANSI_RED "\x1b[0;31m"
#define ANSI_GREEN "\x1b[0;32m"
#define ANSI_YELLOW "\x1b[0;33m"
#define ANSI_BLUE "\x1b[0;34m"
#define ANSI_MAGENTA "\x1b[0;35m"
#define ANSI_CYAN "\x1b[0;36m"
#define ANSI_WHITE "\x1b[0;37m"

#if defined(LOG_RAYLIB)
#include <raylib.h>
#define __LOG_COMMON(_level, _color, _msg, ...)                                \
    (TraceLog(_level,                                                          \
              _color __FILE__ ":" STR(__LINE__) ":%s: " _msg ANSI_RESET,       \
              __func__, ##__VA_ARGS__))
#elif defined(LOG_STDLIB)
#include <stdio.h>
#define __LOG_COMMON(_level, _color, _msg, ...)                                \
    (fprintf(stderr,                                                           \
             _color "[" #_level "] " __FILE__                                  \
                    ":" STR(__LINE__) ":%s: " _msg ANSI_RESET "\n",            \
             __func__, ##__VA_ARGS__))
#elif defined(LOG_NONE)
#define __LOG_COMMON(...)
#else
#define __LOG_COMMON(...)
#error at least one log backend must be defined
#endif

#define LOG_DBG(msg, ...) __LOG_COMMON(LOG_DEBUG, ANSI_BLUE, msg, ##__VA_ARGS__)
#define LOG_INF(msg, ...) __LOG_COMMON(LOG_INFO, ANSI_GREEN, msg, ##__VA_ARGS__)
#define LOG_WRN(msg, ...)                                                      \
    __LOG_COMMON(LOG_WARNING, ANSI_YELLOW, msg, ##__VA_ARGS__)
#define LOG_ERR(msg, ...) __LOG_COMMON(LOG_ERROR, ANSI_RED, msg, ##__VA_ARGS__)

#endif /* LOG_H */
