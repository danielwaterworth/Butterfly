/*  
    Copyright (C) 2011 Butterfly authors,
    
    This file is part of Butterfly.
    
    Butterfly is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Butterfly is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "string_type.h"

#include "assert.h"
#include "stdlib.h"

void str_convert(const char *in, char_t *out, uint32_t out_sz) {
    uint32_t i = 0;
    while (1) {
        assert((i < out_sz) && "cannot convert string, output buffer too small");
        assert(in[i] < 128);
        out[i] = in[i];
        if (in[i] == 0) {
            return;
        }
        ++i;
    }
}

#ifdef BUTTERFLY_USE_ICU
char_t *str_strdup(const char_t *in) {
    uint32_t len = u_strlen(in) + 1;
    UChar *result = malloc(sizeof(UChar) * len);
    u_memcpy(result, in, len);
    return result;
}

int str_strcmp(const char_t *a, const char_t *b) {
    return u_strcmp(a, b);
}

uint32_t str_next(const char_t *str, uint32_t *i, uint32_t sz) {
    uint32_t c;
    U16_NEXT(str, *i, sz, c);
    return c;
}

void str_append(char_t *str, uint32_t *i, uint32_t ch) {
    U16_APPEND_UNSAFE(str, *i, ch);
}

short str_encoding_length(uint32_t ch) {
    return U16_LENGTH(ch);
}

uint32_t str_strlen(const char_t *str) {
    return u_strlen(str);
}

char_t *str_strcpy(char_t *dst, const char_t *src) {
    return u_strcpy(dst, src);
}

int str_memcmp(const char_t *dst, const char_t *src, uint32_t sz) {
    return u_memcmp(dst, src, sz);
}
#endif

#ifdef BUTTERFLY_USE_ASCII
char_t *str_strdup(const char_t *str) {
    return strdup(str);
}

int str_strcmp(const char_t *a, const char_t *b) {
    return strcmp(a, b);
}

uint32_t str_next(const char_t *str, uint32_t *i, uint32_t sz) {
    return str[(*i)++];
}

void str_append(char_t *str, uint32_t *i, uint32_t ch) {
    assert (ch < 256);
    str[(*i)++] = ch;
}

short str_encoding_length(uint32_t ch) {
    return 1;
}

uint32_t str_strlen(const char_t *str) {
    return strlen(str);
}

char_t *str_strcpy(char_t *dst, const char_t *src) {
    return strcpy(dst, src);
}

int str_memcmp(const char_t *dst, const char_t *src, uint32_t sz) {
    return memcmp(dst, src, sz);
}
#endif
