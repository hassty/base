#ifndef BASE_STRING_H
#define BASE_STRING_H

#include "base_memory.h"
#include "util.h"
#include <stdarg.h>

typedef struct {
    u8 *str;
    u64 size;
} str8_t;

static inline str8_t str8(u8 *str, u64 size) {
    return (str8_t){
        .str = str,
        .size = size,
    };
}

static inline str8_t str8_range(u8 *first, u8 *opl) {
    return (str8_t){
        .str = first,
        .size = (u64)(opl - first),
    };
}
str8_t str8_cstring(u8 *cstr);

#define STR8_LITERAL(s) str8((U8 *)(s), sizeof(s) - 1)

str8_t str8_prefix(str8_t str, u64 size);
str8_t str8_chop(str8_t str, u64 amount);
str8_t str8_postfix(str8_t str, u64 size);
str8_t str8_skip(str8_t str, u64 amount);
str8_t str8_substr(str8_t str, u64 first, u64 opl); // TODO

#define STR8_EXPAND(s) (int)((s).size), ((s).str)

typedef struct str8_node_s {
    struct str8_node_s *next;
    str8_t string;
} str8_node_t;

typedef struct {
    str8_node_t *first;
    str8_node_t *last;
    u64 node_count;
    u64 total_size;
} str8_list_t;

typedef struct {
    str8_t pre;
    str8_t mid;
    str8_t post;
} string_join;

typedef u32 str_match_flags;
enum {
    STR_MATCH_FLAG_NO_CASE = 1 << 0,
};

bool str8_match(str8_t a, str8_t b, str_match_flags flags);

u8 str8_char_uppercase(u8 c);
u8 str8_char_lowercase(u8 c);

void str8_list_push_explicit(str8_list_t *list, str8_t string,
                             str8_node_t *node_memory);
void str8_list_push(m_arena *arena, str8_list_t *list, str8_t string);
str8_t str8_join(m_arena *arena, str8_list_t *list, string_join *join_optional);
str8_list_t str8_split(m_arena *arena, str8_t string, u8 *split_characters,
                       u32 count);

str8_t str8_pushfv(m_arena *arena, char *fmt, va_list args);
str8_t str8_pushf(m_arena *arena, char *fmt, ...);
void str8_list_pushf(m_arena *arena, str8_list_t *list, char *fmt, ...);

typedef struct {
    u32 codepoint;
    u32 size;
} string_decode_t;

string_decode_t str_decode_utf8(u8 *str, u32 capacity);
u32 str_encode_utf8(u8 *dst, u32 codepoint);

typedef struct {
    u32 *str;
    u64 size;
} str32_t;

str32_t str32_from_str8(m_arena *arena, str8_t string);
str8_t str8_from_str32(m_arena *arena, str32_t string);

typedef struct {
    u16 *str;
    u64 size;
} str16_t;

string_decode_t str_decode_utf16(u16 *str, u32 capacity);
u32 str_encode_utf16(u16 *dst, u32 codepoint);
str16_t str16_from_str8(m_arena *arena, str8_t string);
str8_t str8_from_str16(m_arena *arena, str16_t string);

#endif /* BASE_STRING_H */
