#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define ARG_UNUSED(arg) ((void)(arg))

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

#ifndef NO_STDLIB
#include <string.h>
#define MEMORY_ZERO(x, size) memset((x), 0, (size))
#define MEMORY_ZERO_STRUCT(x) MEMORY_ZERO((x), sizeof(*(x)))
#define MEMORY_ZERO_ARRAY(arr) MEMORY_ZERO((arr), sizeof(arr))
#define MEMORY_ZERO_TYPED(x, count) MEMORY_ZERO((x), sizeof(*(x)) * count)

#define MEMORY_MATCH(a, b, size) (memcmp((a), (b), (size)) == 0)

#define MEMORY_COPY(dst, src, size) memmove((dst), (src), (size))
#define MEMORY_COPY_STRUCT(dst, src)                                           \
    MEMORY_COPY((dst), (src), MIN(sizeof(*(dst)), sizeof(*(src))))
#define MEMORY_COPY_ARRAY(dst, src)                                            \
    MEMORY_COPY((dst), (src), MIN(sizeof(dst), sizeof(src)))
#define MEMORY_COPY_TYPED(dst, src, count)                                     \
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
#define UINT_TO_POINTER(x) ((void *)(uintptr_t)(x))
#define POINTER_TO_INT(x) ((intptr_t)(x))
#define INT_TO_POINTER(x) ((void *)(intptr_t)(x))

#define MEMBER(type, member) (((type *)(0))->member)
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
#define CLAMP(min, x, max)                                                     \
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

#define DEFER_LOOP(begin, end)                                                 \
    for (int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))

#define DLL_PUSH_BACK_NP(f, l, n, next, prev)                                  \
    (((f) == 0)                                                                \
         ? ((f) = (l) = (n), (n)->next = (n)->prev = 0)                        \
         : ((n)->prev = (l), (l)->next = (n), (l) = (n), (n)->next = 0))
#define DLL_PUSH_BACK(f, l, n) DLL_PUSH_BACK_NP(f, l, n, next, prev)
#define DLL_PUSH_FRONT(f, l, n) DLL_PUSH_BACK_NP(l, f, n, prev, next)
#define DLL_REMOVE_NP(f, l, n, next, prev)                                     \
    (((f) == (n)) ? ((f) = (f)->next, (f)->prev = 0)                           \
     : ((l) == (n))                                                            \
         ? ((l) = (l)->prev, (l)->next = 0)                                    \
         : ((n)->next->prev = (n)->prev, (n)->prev->next = (n)->next))
#define DLL_REMOVE(f, l, n) DLL_REMOVE_NP(f, l, n, next, prev)

#define SLL_QUEUE_PUSH_N(f, l, n, next)                                        \
    (((f) == 0) ? ((f) = (l) = (n))                                            \
                : (((l)->next = (n), (l) = (n)), (n)->next = 0))
#define SLL_QUEUE_PUSH(f, l, n) SLL_QUEUE_PUSH_N(f, l, n, next)
#define SLL_QUEUE_PUSH_FRONT_N(f, l, n, next)                                  \
    (((f) == 0) ? ((f) = (l) = (n), (n)->next = 0)                             \
                : ((n)->next = (f), (f) = (n)))
#define SLL_QUEUE_PUSH_FRONT(f, l, n) SLL_QUEUE_PUSH_FRONT_N(f, l, n, next)
#define SLL_QUEUE_POP_N(f, l, next)                                            \
    (((f) == (l)) ? ((f) = (l) = 0) : ((f) = (f)->next))
#define SLL_QUEUE_POP(f, l) SLL_QUEUE_POP_N(f, l, next)

#define SLL_STACK_PUSH_N(f, n, next) ((n)->next = (f), (f) = (n))
#define SLL_STACK_PUSH(f, n) SLL_STACK_PUSH_N(f, n, next)
#define SLL_STACK_POP_N(f, next) (((f) == 0) ? 0 : ((f) = (f)->next))
#define SLL_STACK_POP(f) SLL_STACK_POP_N(f, next)

#endif /* UTIL_H */
