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

#ifndef STRING_TYPE_H
#define STRING_TYPE_H

#include "stdint.h"

#ifndef BUTTERFLY_USE_ICU
#ifndef BUTTERFLY_USE_ASCII
#error define either BUTTERFLY_USE_ICU or BUTTERFLY_USE_ASCII
#endif
#endif

#ifdef BUTTERFLY_USE_ICU
#include "ustring.h"

typedef UChar char_t;
#endif

#ifdef BUTTERFLY_USE_ASCII
#include "string.h"

typedef char char_t;
#endif

#define STR_INIT(VAR, STR, SZ) \
char_t VAR[SZ + 1]; \
str_convert(STR, VAR, SZ + 1)

void str_convert(const char *, char_t *, uint32_t);
char_t *str_strdup(const char_t *);
int str_strcmp(const char_t *, const char_t *);
int str_memcmp(const char_t *, const char_t *, uint32_t);
uint32_t str_next(const char_t *, uint32_t *, uint32_t);
void str_append(char_t *, uint32_t *, uint32_t);
short str_encoding_length(uint32_t);
uint32_t str_strlen(const char_t *);
char_t *str_strcpy(char_t *, const char_t *);
#endif
