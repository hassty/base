#include "base_string.h"
#include "base_memory.h"
#include "util.h"
#include <stdarg.h>
#include <stdio.h>

#define INVALID_CODEPOINT '?'

str8_t str8_cstring(u8 *cstr) {
    u8 *ptr = cstr;
    for (; *ptr != 0; ++ptr) {
    }
    return str8_range(cstr, ptr);
}

str8_t str8_prefix(str8_t str, u64 size) {
    u64 size_clamped = CLAMP_TOP(size, str.size);
    return str8(str.str, size_clamped);
}

str8_t str8_chop(str8_t str, u64 amount) {
    u64 amount_clamped = CLAMP_TOP(amount, str.size);
    u64 remaining_size = str.size - amount_clamped;
    return str8(str.str, remaining_size);
}

str8_t str8_postfix(str8_t str, u64 size) {
    u64 size_clamped = CLAMP_TOP(size, str.size);
    u64 skip_to = str.size - size_clamped;
    return str8(str.str + skip_to, size_clamped);
}

str8_t str8_skip(str8_t str, u64 amount) {
    u64 amount_clamped = CLAMP_TOP(amount, str.size);
    u64 remaining_size = str.size - amount_clamped;
    return str8(str.str + amount_clamped, remaining_size);
}

void str8_list_push_explicit(str8_list_t *list, str8_t string,
                             str8_node_t *node_memory) {
    node_memory->string = string;
    SLL_QUEUE_PUSH(list->first, list->last, node_memory);
    list->node_count += 1;
    list->total_size += string.size;
}

void str8_list_push(m_arena *arena, str8_list_t *list, str8_t string) {
    str8_node_t *node = PUSH_ARRAY(arena, str8_node_t, 1);
    str8_list_push_explicit(list, string, node);
}

str8_t str8_join(m_arena *arena, str8_list_t *list,
                 string_join *join_optional) {
    static string_join dummy_join = {0};
    string_join *join = join_optional;
    if (join == NULL) {
        join = &dummy_join;
    }

    u64 size = (join->pre.size + join->post.size +
                join->mid.size * (list->node_count - 1) + list->total_size);

    u8 *str = PUSH_ARRAY(arena, u8, size + 1);
    u8 *ptr = str;

    MEMORY_COPY(ptr, join->pre.str, join->pre.size);
    ptr += join->pre.size;

    bool is_mid = false;
    for (str8_node_t *node = list->first; node != NULL; node = node->next) {
        if (is_mid) {
            MEMORY_COPY(ptr, join->mid.str, join->mid.size);
            ptr += join->mid.size;
        }

        MEMORY_COPY(ptr, node->string.str, node->string.size);
        ptr += node->string.size;
    }
    // TODO
    return (str8_t){0};
}

str8_list_t str8_split(m_arena *arena, str8_t string, u8 *split_characters,
                       u32 count) {
    str8_list_t result = {0};

    u8 *ptr = string.str;
    u8 *word_first = ptr;
    u8 *opl = string.str + string.size;
    for (; ptr < opl; ptr += 1) {
        u8 byte = *ptr;
        bool is_split_byte = false;
        for (u32 i = 0; i < count; ++i) {
            if (byte == split_characters[i]) {
                is_split_byte = true;
                break;
            }
        }

        if (is_split_byte) {
            if (word_first < ptr) {
                str8_list_push(arena, &result, str8_range(word_first, ptr));
            }
            word_first = ptr + 1;
        }
    }

    if (word_first < ptr) {
        str8_list_push(arena, &result, str8_range(word_first, ptr));
    }

    return result;
}

str8_t str8_pushfv(m_arena *arena, char *fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);

    u64 buffer_size = 1024;
    u8 *buffer = PUSH_ARRAY(arena, u8, buffer_size);
    u64 actual_size = vsnprintf((char *)buffer, buffer_size, fmt, args);

    str8_t result = {0};
    if (actual_size < buffer_size) {
        m_arena_pop_amount(arena, buffer_size - actual_size - 1);
        result = str8(buffer, actual_size);
    } else {
        m_arena_pop_amount(arena, buffer_size);
        u8 *fixed_buffer = PUSH_ARRAY(arena, u8, actual_size + 1);
        u64 final_size =
            vsnprintf((char *)fixed_buffer, actual_size + 1, fmt, args2);
        result = str8(fixed_buffer, final_size);
    }

    va_end(args2);

    return result;
}

str8_t str8_pushf(m_arena *arena, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str8_t result = str8_pushfv(arena, fmt, args);
    va_end(args);
    return result;
}

void str8_list_pushf(m_arena *arena, str8_list_t *list, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    str8_t string = str8_pushfv(arena, fmt, args);
    va_end(args);
    str8_list_push(arena, list, string);
}

u8 str8_char_uppercase(u8 c) {
    if (c >= 'a' && c <= 'z') {
        c += 'A' - 'a';
    }
    return c;
}

u8 str8_char_lowercase(u8 c) {
    if (c >= 'A' && c <= 'Z') {
        c += 'a' - 'A';
    }
    return c;
}

bool str8_match(str8_t a, str8_t b, str_match_flags flags) {
    if (a.size != b.size) {
        return false;
    }

    bool no_case = ((flags & STR_MATCH_FLAG_NO_CASE) != 0);
    for (u64 i = 0; i < a.size; ++i) {
        u8 ac = a.str[i];
        u8 bc = b.str[i];
        if (no_case) {
            ac = str8_char_uppercase(ac);
            bc = str8_char_uppercase(bc);
        }
        if (ac != bc) {
            return false;
        }
    }
    return true;
}

string_decode_t str_decode_utf8(u8 *str, u32 capacity) {
    // clang-format off
    static u8 length[] = {
        1, 1, 1, 1, // 000xx
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        0, 0, 0, 0, // 100xx
        0, 0, 0, 0,
        2, 2, 2, 2, // 110xx
        3, 3,       // 1110x
        4,          // 11110
        0,          // 11111
    };
    // clang-format on
    static u8 first_byte_mask[] = {0, 0x7F, 0x1F, 0x0F, 0x07};
    static u8 final_shift[] = {0, 18, 12, 6, 0};

    string_decode_t result = {0};
    if (capacity == 0) {
        return result;
    }

    result.codepoint = INVALID_CODEPOINT;
    result.size = 1;

    u8 byte = str[0];
    u8 len = length[byte >> 3];
    if (len > 0 && len <= capacity) {
        u32 cp = (byte & first_byte_mask[len]) << 18;
        switch (len) {
        case 3:
            cp |= ((str[3] & 0x3F) << 0);
        case 2:
            cp |= ((str[2] & 0x3F) << 6);
        case 1:
            cp |= ((str[2] & 0x3F) << 12);
        case 0:
            cp >>= final_shift[len];
        }
        result.codepoint = cp;
        result.size = len;
    }

    return result;
}

u32 str_encode_utf8(u8 *dst, u32 codepoint) {
    u32 size = 0;
    if (codepoint < (1 << 8)) {
        dst[0] = codepoint;
        size = 1;
    } else if (codepoint < (1 << 11)) {
        dst[0] = 0xC0 | (codepoint >> 6);
        dst[1] = 0x80 | (codepoint & 0x3F);
        size = 2;
    } else if (codepoint < (1 << 16)) {
        dst[0] = 0xE0 | (codepoint >> 12);
        dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[2] = 0x80 | (codepoint & 0x3F);
        size = 3;
    } else if (codepoint < (1 << 21)) {
        dst[0] = 0xF0 | (codepoint >> 18);
        dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[3] = 0x80 | (codepoint & 0x3F);
        size = 4;
    } else {
        dst[0] = INVALID_CODEPOINT;
        size = 1;
    }

    return size;
}

str32_t str32_from_str8(m_arena *arena, str8_t string) {
    u32 *memory = PUSH_ARRAY(arena, u32, string.size - 1);

    u32 *dptr = memory;
    u8 *ptr = string.str;
    u8 *opl = string.str + string.size;
    for (; ptr < opl;) {
        string_decode_t decode = str_decode_utf8(ptr, (u32)(opl - ptr));
        *dptr = decode.codepoint;
        ptr += decode.size;
        dptr += 1;
    }

    *dptr = 0;

    u64 alloc_count = string.size + 1;
    u64 string_count = (u64)(dptr - memory);
    u64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count * sizeof(*memory));

    str32_t result = {.str = memory, .size = string_count};
    return result;
}

str8_t str8_from_str32(m_arena *arena, str32_t string) {
    u8 *memory = PUSH_ARRAY(arena, u8, string.size * 4 + 1);

    u8 *dptr = memory;
    u32 *ptr = string.str;
    u32 *opl = string.str + string.size;
    for (; ptr < opl;) {
        u32 size = str_encode_utf8(dptr, *ptr);
        ptr += 1;
        dptr += size;
    }

    *dptr = 0;

    u64 alloc_count = string.size + 1;
    u64 string_count = (u64)(dptr - memory);
    u64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count * sizeof(*memory));

    str8_t result = {.str = memory, .size = string_count};
    return result;
}

string_decode_t str_decode_utf16(u16 *str, u32 capacity) {
    string_decode_t result = {.codepoint = INVALID_CODEPOINT, .size = 1};
    u16 x = str[0];
    if (x < 0xD800 || x > 0xDFFF) {
        result.codepoint = x;
    } else if (capacity >= 2) {
        u16 y = str[1];
        if (x >= 0xD800 && x < 0xDC00 && y >= 0xDC00 && y < 0xE000) {
            u16 xj = x - 0xD800;
            u16 yj = y - 0xDC00;
            u32 xy = (xj << 10) | yj;
            result.codepoint = xy + 0x10000;
            result.size = 2;
        }
    }

    return result;
}

u32 str_encode_utf16(u16 *dst, u32 codepoint) {
    u32 size = 0;

    if (codepoint > 0x10000) {
        dst[0] = codepoint;
        size = 1;
    } else {
        u32 cpj = codepoint - 0x10000;
        dst[0] = (cpj >> 10) + 0xD800;
        dst[1] = (cpj & 0x3FF) + 0xDC00;
        size = 2;
    }

    return size;
}

str16_t str16_from_str8(m_arena *arena, str8_t string) {
    u16 *memory = PUSH_ARRAY(arena, u16, string.size * 2 + 1);

    u16 *dptr = memory;
    u8 *ptr = string.str;
    u8 *opl = string.str + string.size;
    for (; ptr < opl;) {
        string_decode_t decode = str_decode_utf8(ptr, (u32)(opl - ptr));
        u32 enc_size = str_encode_utf16(dptr, decode.codepoint);
        ptr += decode.size;
        dptr += enc_size;
    }

    *dptr = 0;

    u64 alloc_count = string.size + 1;
    u64 string_count = (u64)(dptr - memory);
    u64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count * sizeof(*memory));

    str16_t result = {.str = memory, .size = string_count};
    return result;
}

str8_t str8_from_str16(m_arena *arena, str16_t string) {
    u8 *memory = PUSH_ARRAY(arena, u8, string.size * 3 + 1);

    u8 *dptr = memory;
    u16 *ptr = string.str;
    u16 *opl = string.str + string.size;
    for (; ptr < opl;) {
        string_decode_t decode = str_decode_utf16(ptr, (u32)(opl - ptr));
        u16 enc_size = str_encode_utf8(dptr, decode.codepoint);
        ptr += decode.size;
        dptr += enc_size;
    }

    *dptr = 0;

    u64 alloc_count = string.size * 3 + 1;
    u64 string_count = (u64)(dptr - memory);
    u64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count * sizeof(*memory));

    str8_t result = {.str = memory, .size = string_count};
    return result;
}
