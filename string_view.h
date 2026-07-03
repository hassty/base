#pragma once

#include "base.h"

#ifdef LANG_CPP
extern "C" {
#endif

typedef struct {
    const char *data;
    usize length;
} string_view;

#define LITERAL_LENGTH(literal) (sizeof(literal) - 1)

#ifdef LANG_CPP
#define SV(str, str_len) (__sv__(str, str_len))
#define SV_CSTR(cstr) (__sv__(cstr, strlen(cstr)))
#define SV_LIT(literal) (__sv__(literal, LITERAL_LENGTH(literal)))

static inline string_view __sv__(const char *str, usize str_len) {
    string_view result{};
    result.data = str;
    result.length = str_len;
    return result;
}
#else
#define SV(str, str_len) ((string_view){.data = (str), .length = (str_len)})
#define SV_CSTR(cstr) ((string_view){.data = (cstr), .length = strlen(cstr)})
#define SV_LIT(literal) ((string_view){.data = (literal), .length = LITERAL_LENGTH(literal)})
#endif

#define SV_NIL SV_LIT("")
#define SV_FORMAT "%.*s"
#define SV_EXPAND(sv) (i32)(sv).length, (sv).data

bool sv_to_cstr_buf(string_view sv, char *buf, usize buf_size);
bool sv_is_equal(string_view a, string_view b);
void sv_chop_right_inplace(string_view *sv, usize count);
string_view sv_chop_right(string_view sv, usize count);
void sv_chop_left_inplace(string_view *sv, usize count);
string_view sv_chop_left(string_view sv, usize count);
void sv_trim_right_inplace(string_view *sv);
string_view sv_trim_right(string_view sv);
void sv_trim_left_inplace(string_view *sv);
string_view sv_trim_left(string_view sv);
void sv_trim_inplace(string_view *sv);
string_view sv_trim(string_view sv);
string_view sv_find_substr(string_view sv, string_view substr);
string_view sv_find_substr_cstr(string_view sv, const char *substr);
string_view sv_split_by_delimiter(string_view *rest, char delimiter);
u8 char_to_digit(char c);
char digit_to_char(u8 digit);
bool sv_to_u64(string_view sv, u64 *result);
u64 sv_to_u64_or(string_view sv, u64 default_value);
bool sv_to_i64(string_view sv, i64 *result);
i64 sv_to_i64_or(string_view sv, i64 default_value);
bool sv_to_f64(string_view sv, f64 *result);
f64 sv_to_f64_or(string_view sv, f64 default_value);

#ifdef LANG_CPP
} // extern "C"
#endif

#ifdef STRING_VIEW_IMPLEMENTATION
#include <ctype.h>

bool sv_to_cstr_buf(string_view sv, char *buf, usize buf_size) {
    ASSERT(buf, "sv_to_cstr_buf: buf is NULL");
    ASSERT(buf_size >= sv.length + 1, "sv_to_cstr_buf: buf_size is to small");

    if (!buf || buf_size < sv.length + 1) {
        // TODO: maybe MEMORY_ZERO(buf, buf_size)?
        return false;
    }

    for (usize i = 0; i < sv.length; ++i) {
        buf[i] = sv.data[i];
    }
    buf[sv.length] = '\0';

    return true;
}

bool sv_is_equal(string_view a, string_view b) {
    if (a.length != b.length) {
        return false;
    } else if (a.data == b.data) {
        return true;
    }

    for (usize i = 0; i < a.length; ++i) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }

    return true;
}

void sv_chop_right_inplace(string_view *sv, usize count) {
    ASSERT(sv, "sv_chop_right_inplace: sv is NULL");
    count = CLAMP_TOP(count, sv->length);
    sv->length -= count;
}

string_view sv_chop_right(string_view sv, usize count) {
    sv_chop_right_inplace(&sv, count);
    return sv;
}

void sv_chop_left_inplace(string_view *sv, usize count) {
    ASSERT(sv, "sv_chop_left_inplace: sv is NULL");
    count = CLAMP_TOP(count, sv->length);
    sv->data += count;
    sv->length -= count;
}

string_view sv_chop_left(string_view sv, usize count) {
    sv_chop_left_inplace(&sv, count);
    return sv;
}

void sv_trim_right_inplace(string_view *sv) {
    ASSERT(sv, "sv_trim_right_inplace: sv is NULL");
    while (sv->length > 0 && isspace(sv->data[sv->length - 1])) {
        sv_chop_right_inplace(sv, 1);
    }
}

string_view sv_trim_right(string_view sv) {
    sv_trim_right_inplace(&sv);
    return sv;
}

void sv_trim_left_inplace(string_view *sv) {
    ASSERT(sv, "sv_trim_left_inplace: sv is NULL");
    while (sv->length > 0 && isspace(sv->data[0])) {
        sv_chop_left_inplace(sv, 1);
    }
}

string_view sv_trim_left(string_view sv) {
    sv_trim_left_inplace(&sv);
    return sv;
}

void sv_trim_inplace(string_view *sv) {
    ASSERT(sv, "sv_trim_inplace: sv is NULL");
    sv_trim_left_inplace(sv);
    sv_trim_right_inplace(sv);
}

string_view sv_trim(string_view sv) {
    sv_trim_inplace(&sv);
    return sv;
}

string_view sv_find_substr(string_view sv, string_view substr) {
    while (sv.length > substr.length) {
        string_view window = SV(sv.data, substr.length);
        if (sv_is_equal(window, substr)) {
            return sv;
        }
        sv_chop_left_inplace(&sv, 1);
    }

    if (sv_is_equal(sv, substr)) {
        return sv;
    }

    return SV_NIL;
}

// TODO: is it really needed?
string_view sv_find_substr_cstr(string_view sv, const char *substr) {
    ASSERT(substr, "sv_find_substr_cstr: substr is NULL");
    return sv_find_substr(sv, SV_CSTR(substr));
}

string_view sv_split_by_delimiter(string_view *rest, char delimiter) {
    ASSERT(rest, "sv_split_by_delimiter: rest is NULL");

    while (rest->data[0] == delimiter) {
        sv_chop_left_inplace(rest, 1);
    }

    for (usize i = 0; i < rest->length; ++i) {
        char c = rest->data[i];
        if (c == delimiter) {
            string_view result = SV(rest->data, i);
            sv_chop_left_inplace(rest, i);
            return result;
        }
    }

    string_view result = *rest;
    sv_chop_left_inplace(rest, rest->length);
    return result;
}

u8 char_to_digit(char c) {
    return c - '0';
}

char digit_to_char(u8 digit) {
    return digit + '0';
}

bool sv_to_u64(string_view sv, u64 *result) {
    ASSERT(result, "sv_to_u64: result is NULL");
    ASSERT(*result == 0, "sv_to_u64: result must be initialized to 0");
    *result = 0; // just in case

    if (sv_is_equal(sv, SV_NIL) || (sv.data[0] == '0' && sv.length > 1)) {
        *result = 0;
        return false;
    }

    u64 mul = 1;
    while (sv.length > 0) {
        char c = sv.data[sv.length - 1];
        if (!isdigit(c)) {
            *result = 0;
            return false;
        }

        *result += mul * char_to_digit(c);
        mul *= 10;
        sv_chop_right_inplace(&sv, 1);
    }

    return true;
}

u64 sv_to_u64_or(string_view sv, u64 default_value) {
    u64 result = 0;
    return sv_to_u64(sv, &result) ? result : default_value;
}

bool sv_to_i64(string_view sv, i64 *result) {
    ASSERT(result, "sv_to_i64: result is NULL");
    ASSERT(*result == 0, "sv_to_i64: result must be initialized to 0");
    *result = 0; // just in case

    i64 sign = 1;
    if (sv.data[0] == '-') {
        sign = -1;
        sv_chop_left_inplace(&sv, 1);
    }

    if (!sv_to_u64(sv, (u64*)result)) {
        return false;
    }

    *result *= sign;
    return true;
}

i64 sv_to_i64_or(string_view sv, i64 default_value) {
    i64 result = 0;
    return sv_to_i64(sv, &result) ? result : default_value;
}

bool sv_to_f64(string_view sv, f64 *result) {
    ASSERT(result, "sv_to_f64: result is NULL");
    ASSERT(*result == 0, "sv_to_f64: result must be initialized to 0");
    *result = 0; // just in case

    if (sv_is_equal(sv, SV_NIL)) {
        return false;
    }

    f64 sign = 1;
    if (sv.data[0] == '-') {
        sign = -1;
        sv_chop_left_inplace(&sv, 1);
    }

    string_view integer_part = sv_split_by_delimiter(&sv, '.');
    string_view fractional_part = sv_chop_left(sv, 1);

    f64 div = 10;
    while (fractional_part.length > 0) {
        char c = fractional_part.data[0];
        if (!isdigit(c)) {
            *result = 0;
            return false;
        }

        *result += char_to_digit(c) / div;
        div *= 10;
        sv_chop_left_inplace(&fractional_part, 1);
    }

    f64 mul = 1;
    while (integer_part.length > 0) {
        char c = integer_part.data[integer_part.length - 1];
        if (!isdigit(c)) {
            *result = 0;
            return false;
        }

        *result += char_to_digit(c) * mul;
        mul *= 10;
        sv_chop_right_inplace(&integer_part, 1);
    }

    *result *= sign;
    return true;
}

f64 sv_to_f64_or(string_view sv, f64 default_value) {
    f64 result = 0.0;
    return sv_to_f64(sv, &result) ? result : default_value;
}
#endif
