/*  
    Copyright (C) 2011 Daniel Waterworth
    
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

#ifndef OBJECT_H
#define OBJECT_H

#include "stdint.h"
#include "stdbool.h"

#include "ustring.h"

#define U_STRING(name, str, len) \
    U_STRING_DECL(name, str, len); \
    U_STRING_INIT(name, str, len)

struct object;
typedef struct object object;

#define OBJECT_MAP 1
#define OBJECT_LIST 2
#define OBJECT_STR 3
#define OBJECT_INT 4
#define OBJECT_FLOAT 5
#define OBJECT_NONE 6
#define OBJECT_BOOL 7

object *object_map();
object *object_list();
object *object_str(UChar *);
object *object_int(int64_t);
object *object_float(double);
object *object_none();
object *object_bool(bool);

int object_type(object *);
object *object_copy(object *);
void object_free(object *);
bool object_hashable(object *);
uint32_t object_hash(object *);
bool object_eq(object *, object *);

void object_map_set(object *, object *, object *);
void object_map_rem(object *, object *);
object *object_map_get(object *, object *);
void object_map_clear(object *);

void object_list_set(object *, int32_t, object *);
void object_list_insert_at(object *, int32_t, object *);
void object_list_remove(object *, int32_t);
object *object_list_get(object *, int32_t);
int32_t object_list_length(object *);

UChar *object_str_get(object *);
int64_t object_int_get(object *);
double object_float_get(object *);
bool object_bool_get(object *);

UChar *object_join(object *);

UChar *object_to_json(object *, bool);
object *object_from_json(UChar *);

#endif
