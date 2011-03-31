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

#ifndef OBJECT_H
#define OBJECT_H

#include "stdint.h"
#include "stdbool.h"
#include "string_type.h"

struct object;
typedef struct object object;

struct object_iterator;
typedef struct object_iterator object_iterator;

#define OBJECT_MAP 1
#define OBJECT_LIST 2
#define OBJECT_STR 3
#define OBJECT_INT 4
#define OBJECT_FLOAT 5
#define OBJECT_NONE 6
#define OBJECT_BOOL 7

object *object_map();
object *object_list();
object *object_str(char_t *);
object *object_int(int64_t);
object *object_float(double);
object *object_none();
object *object_bool(bool);

int object_type(object *);
object *object_copy(object *);
void object_free(object *);
bool object_hashable(object *);
bool object_iterable(object *);
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

char_t *object_str_get(object *);
int64_t object_int_get(object *);
double object_float_get(object *);
bool object_bool_get(object *);

object_iterator *object_iterate(object *);
bool object_iterator_hasnext(object_iterator *);
object *object_iterator_getnext(object_iterator *);
void object_iterator_free(object_iterator *);

char_t *object_join(object *);

char_t *object_to_json(object *, bool);
object *object_from_json(const char_t *);

#endif
