#ifndef BASE_H
#define BASE_H

#include <stdbool.h>

#define UNUSED(arg) ((void)(arg))

#define ARRAY_LENGTH(arr) (sizeof((arr)) / sizeof((arr)[0]))

#ifndef NO_STDLIB
#include <string.h>
#define MEMORY_ZERO(x, size) memset((x), 0, (size))
#define MEMORY_ZERO_STRUCT(x) MEMORY_ZERO((x), sizeof(*(x)))
#define MEMORY_ZERO_ARRAY(arr) MEMORY_ZERO((arr), sizeof(arr))
#define MEMORY_ZERO_TYPED(x, count) MEMORY_ZERO((x), sizeof(*(x)) * count)

#define MEMORY_MATCH(a, b, size) (memcmp((a), (b), (size)) == 0)

#define MEMORY_COPY(dst, src, size) \
    memmove((dst), (src), (size))
#define MEMORY_COPY_STRUCT(dst, src) \
    MEMORY_COPY((dst), (src), MIN(sizeof(*(dst)), sizeof(*(src))))
#define MEMORY_COPY_ARRAY(dst, src) \
    MEMORY_COPY((dst), (src), MIN(sizeof(dst), sizeof(src)))
#define MEMORY_COPY_TYPED(dst, src, count) \
    MEMORY_COPY((dst), (src), MIN(sizeof(*(dst)), sizeof(*(src))) * (count))
#endif /* NO_STDLIB */

#ifdef ENABLE_ASSERT
#ifndef ASSERT
#include <assert.h>
#define ASSERT(x, msg) (assert((x) && msg))
#endif
#else
#define ASSERT(x, msg)
#endif /* ENABLE_ASSERT */

#define BITS_PER_BYTE __CHAR_BIT__
#define SIZE_IN_BITS(x) (sizeof(x) * BITS_PER_BYTE)

#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)
#define TB(x) ((x) << 40)

#define ROUND_DIV(a, b) (((a) + ((b) - 1)) / (b))

#define POINTER_TO_UINT(x) ((uintptr_t)(x))
#define UINT_TO_POINTER(x) ((void*)(uintptr_t)(x))
#define POINTER_TO_INT(x) ((intptr_t)(x))
#define INT_TO_POINTER(x) ((void*)(intptr_t)(x))

#define MEMBER(type, member) (((type*)(0))->member)
#define MEMBER_SIZE(type, member) (sizeof(MEMBER(type, member)))
#define MEMBER_OFFSET(type, member) POINTER_TO_UINT(&MEMBER(type, member))

#define STR_(x) #x
#define STR(x) STR_(x)

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(min, x, max) \
    (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))
#define CLAMP_TOP(a, b) MIN(a, b)
#define CLAMP_BOT(a, b) MAX(a, b)
#endif /* CLAMP */

#define ALIGN_UP_POW2(x, p) (((x) + (p) - 1) & ~((p) - 1))
#define ALIGN_DOWN_POW2(x, p) ((x) & ~((p) - 1))

#ifndef NO_STDLIB
#include <stdint.h>
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#include <stddef.h>
typedef size_t usize;
typedef ptrdiff_t isize;

typedef void void_fn_ptr(void);
#endif /* NO_STDLIB */

#define DEFER_LOOP(begin, end) \
    for (int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))

#define DLL_PUSH_BACK_NP_NIL(f, l, n, next, prev, nil)                       \
    (((f) == (nil))                                                          \
         ? ((f) = (l) = (n), (n)->next = (n)->prev = (nil))                  \
         : ((n)->prev = (l), (l)->next = (n), (l) = (n), (n)->next = (nil)))
#define DLL_PUSH_BACK_NIL(f, l, n, nil) DLL_PUSH_BACK_NP_NIL(f, l, n, next, prev, nil)
#define DLL_PUSH_BACK(f, l, n) DLL_PUSH_BACK_NP_NIL(f, l, n, next, prev, NULL)
#define DLL_PUSH_FRONT_NIL(f, l, n, nil) DLL_PUSH_BACK_NP_NIL(l, f, n, prev, next, nil)
#define DLL_PUSH_FRONT(f, l, n) DLL_PUSH_BACK_NP_NIL(l, f, n, prev, next, NULL)
#define DLL_REMOVE_NP_NIL(f, l, n, next, prev, nil)                    \
    (((f) == (l) && (f) == (n)) ? ((f) = (l) = nil)                    \
     : ((f) == (n))             ? ((f) = (f)->next, (f)->prev = nil)   \
     : ((l) == (n))                                                    \
         ? ((l) = (l)->prev, (l)->next = nil)                          \
         : (((n)->prev == nil) ? nil : ((n)->prev->next = (n)->next)), \
           (((n)->next == nil) ? nil : ((n)->next->prev = (n)->prev)))
#define DLL_REMOVE_NIL(f, l, n, nil) DLL_REMOVE_NP_NIL(f, l, n, next, prev, nil)
#define DLL_REMOVE(f, l, n) DLL_REMOVE_NP_NIL(f, l, n, next, prev, NULL)

#define SLL_QUEUE_PUSH_N_NIL(f, l, n, next, nil)                   \
    (((f) == nil) ? ((f) = (l) = (n))                              \
                : (((l)->next = (n), (l) = (n)), (n)->next = nil))
#define SLL_QUEUE_PUSH_NIL(f, l, n, nil) SLL_QUEUE_PUSH_N_NIL(f, l, n, next, nil)
#define SLL_QUEUE_PUSH(f, l, n) SLL_QUEUE_PUSH_N_NIL(f, l, n, next, NULL)
#define SLL_QUEUE_PUSH_FRONT_N_NIL(f, l, n, next, nil) \
    (((f) == nil) ? ((f) = (l) = (n), (n)->next = nil) \
                : ((n)->next = (f), (f) = (n)))
#define SLL_QUEUE_PUSH_FRONT_NIL(f, l, n, nil) SLL_QUEUE_PUSH_FRONT_N_NIL(f, l, n, next, nil)
#define SLL_QUEUE_PUSH_FRONT(f, l, n) SLL_QUEUE_PUSH_FRONT_N_NIL(f, l, n, next, NULL)
#define SLL_QUEUE_POP_N_NIL(f, l, next, nil) \
    (((f) == (l)) ? ((f) = (l) = nil) : ((f) = (f)->next))
#define SLL_QUEUE_POP_NIL(f, l, nil) SLL_QUEUE_POP_N_NIL(f, l, next, nil)
#define SLL_QUEUE_POP(f, l) SLL_QUEUE_POP_N_NIL(f, l, next, NULL)

#define SLL_STACK_PUSH_N(f, n, next) ((n)->next = (f), (f) = (n))
#define SLL_STACK_PUSH(f, n) SLL_STACK_PUSH_N(f, n, next)
#define SLL_STACK_POP_N_NIL(f, next, nil) (((f) == nil) ? nil : ((f) = (f)->next))
#define SLL_STACK_POP_NIL(f, nil) SLL_STACK_POP_N_NIL(f, next, nil)
#define SLL_STACK_POP(f) SLL_STACK_POP_N(f, next, NULL)

typedef struct {
    f32 x;
    f32 y;
} Vec2;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Rgba;

#define HOUR_TO_MIN(hour)  ((hour) * 60ULL)
#define HOUR_TO_SEC(hour)  ((hour) * 3600ULL)
#define MIN_TO_SEC(min)    ((min) * 60ULL)
#define MIN_TO_HOUR(min)   ((min) / 60ULL)
#define SEC_TO_HOUR(sec)   ((sec) / 3600ULL)
#define SEC_TO_MIN(sec)    ((sec) / 60ULL)
#define SEC_TO_MSEC(sec)   ((sec) * 1000ULL)
#define SEC_TO_USEC(sec)   ((sec) * 1000000ULL)
#define SEC_TO_NSEC(sec)   ((sec) * 1000000000ULL)
#define MSEC_TO_SEC(msec)  ((msec) / 1000ULL)
#define MSEC_TO_USEC(msec) ((msec) * 1000ULL)
#define MSEC_TO_NSEC(msec) ((msec) * 1000000ULL)
#define USEC_TO_SEC(usec)  ((usec) / 1000000ULL)
#define USEC_TO_MSEC(usec) ((usec) / 1000ULL)
#define USEC_TO_NSEC(usec) ((usec) * 1000ULL)
#define NSEC_TO_USEC(nsec) ((nsec) / 1000ULL)
#define NSEC_TO_MSEC(nsec) ((nsec) / 1000000ULL)
#define NSEC_TO_SEC(nsec)  ((nsec) / 1000000000ULL)

typedef struct {
    u16 usec; // [0,999]
    u16 msec; // [0,999]
    u8 sec;   // [0,60]
    u8 min;   // [0,60]
    u8 hour;  // [0,60]
} Time;

#define TIME_FORMAT "%02d:%02d:%02d.%03d,%03d"
#define TIME_EXPAND(t) (t)->hour, (t)->min, (t)->sec, (t)->msec, (t)->usec

// TODO: move to separate file?
static inline Time usec_to_time(u64 usec) {
    u8 hour = SEC_TO_HOUR(USEC_TO_SEC(usec));
    usec -= SEC_TO_USEC(HOUR_TO_SEC(hour));
    u8 min = 0;
    if (usec > 0) {
        min = SEC_TO_MIN(USEC_TO_SEC(usec));
        usec -= SEC_TO_USEC(MIN_TO_SEC(min));
    }
    u8 sec = 0;
    if (usec > 0) {
        sec = USEC_TO_SEC(usec);
        usec -= SEC_TO_USEC(sec);
    }
    u16 msec = 0;
    if (usec > 0) {
        msec = USEC_TO_MSEC(usec);
        usec -= MSEC_TO_USEC(msec);
    }

    return (Time){
        .usec = (u16)usec,
        .msec = msec,
        .sec = sec,
        .min = min,
        .hour = hour,
    };
}

static inline u64 time_to_usec(Time t) {
    return SEC_TO_USEC(HOUR_TO_SEC(t.hour)) + SEC_TO_USEC(MIN_TO_SEC(t.min)) + 
           SEC_TO_USEC(t.sec) + MSEC_TO_USEC(t.msec) + t.usec;
}

#endif /* BASE_H */
