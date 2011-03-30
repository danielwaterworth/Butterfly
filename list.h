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

#ifndef LIST_H
#define LIST_H

#include "stdint.h"

#include "object.h"

#define BRANCH 1
#define LEAF 2

struct list;

struct finger_branch {
    struct list *left, *right;
    int64_t nodes;
};
typedef struct finger_branch finger_branch;

struct list {
    unsigned char type;
    union {
        finger_branch branch;
        object *leaf;
    } data;
};
typedef struct list list;

void list_set(finger_branch *, int32_t, object *);
void list_insert_at(finger_branch *, int32_t, object *);
void list_remove(finger_branch *, int32_t);
object *list_get(finger_branch *, int32_t);
int32_t list_length(finger_branch *);
list *list_copy(list *);

#endif
