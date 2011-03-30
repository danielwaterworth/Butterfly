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

#ifndef MAP_H
#define MAP_H

#include "object.h"

struct record {
    object *key;
    object *val;
};
typedef struct record record;

struct map {
    record *data;
    /* the number of records in the allocation */
    uint32_t sz;
    /* the number of elements in the map */
    uint32_t elems;
};
typedef struct map map;

void map_init(map *, uint32_t);
void map_set(map *, object *, object *);
object *map_get(map *, object *);
void map_rem(map *, object *);
void map_clear(map *);
void map_copy(map *, map *);
uint32_t map_length(map *);

#endif
