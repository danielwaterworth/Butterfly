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

#include "stdlib.h"
#include "assert.h"

#include "object.h"
#include "list.h"
#include "map.h"

static UChar *u_strdup(UChar *in) {
    uint32_t len = u_strlen(in) + 1;
    UChar *result = malloc(sizeof(UChar) * len);
    u_memcpy(result, in, len);
    return result;
}

struct object {
    unsigned char type;
    union {
        finger_branch l;
        map m;
        int64_t n;
        double f;
        UChar *str;
        bool b;
    } data;
};

typedef struct {
    unsigned char type;
    bool b;
} bool_object;

unsigned char none_type = OBJECT_NONE;

#define none ((object *)&none_type)

bool_object bool_true = {OBJECT_BOOL, true};
bool_object bool_false = {OBJECT_BOOL, false};

#define object_true ((object *)&bool_true)
#define object_false ((object *)&bool_false)

object *object_map() {
    object *res = malloc(sizeof(map) + 1);
    res->type = OBJECT_MAP;
    map_init(&res->data.m, 16);
    return res;
}

object *object_list() {
    object *obj = malloc(1 + sizeof(finger_branch));
    obj->type = OBJECT_LIST;
    obj->data.l.left = NULL;
    obj->data.l.right = NULL;
    obj->data.l.nodes = 0;
    return obj;
}

object *object_str(UChar *str) {
    object *obj = malloc(1 + sizeof(UChar *));
    obj->type = OBJECT_STR;
    obj->data.str = u_strdup(str);
    return obj;
}

object *object_int(int64_t n) {
    object *obj = malloc(9);
    obj->type = OBJECT_INT;
    obj->data.n = n;
    return obj;
}

object *object_float(double f) {
    object *obj = malloc(9);
    obj->type = OBJECT_FLOAT;
    obj->data.f = f;
    return obj;
}

object *object_none() {
    return none;
}

object *object_bool(bool b) {
    if (b) {
        return object_true;
    } else {
        return object_false;
    }
}

static object *object_list_copy(object *obj) {
    object *res = malloc(1 + sizeof(finger_branch));
    res->type = OBJECT_LIST;
    res->data.l.right = list_copy(obj->data.l.right);
    res->data.l.left = list_copy(obj->data.l.left);
    res->data.l.nodes = obj->data.l.nodes;
    return res;
}

static object *object_map_copy(object *obj) {
    object *res = malloc(sizeof(map) + 1);
    res->type = OBJECT_MAP;
    
    map_copy(&obj->data.m, &res->data.m);
    
    return res;
}

int object_type(object *obj) {
    return obj->type;
}

bool object_hashable(object *obj) {
    if (obj->type == OBJECT_LIST || obj->type == OBJECT_MAP) {
        return false;
    }
    return true;
}

static uint32_t object_str_hash(object *obj) {
    uint32_t out = 5381;
    size_t len = u_strlen(obj->data.str);
    size_t i;
    
    for (i = 0; i < len; ++i) {
        out = out * 33 + obj->data.str[i];
    }
    
    return out;
}

uint32_t object_hash(object *obj) {
    assert(object_hashable(obj));
    switch (obj->type) {
        case OBJECT_NONE:
            return 0;
        case OBJECT_BOOL:
            if (obj == object_true) {
                return 1;
            }
            return 2;
        case OBJECT_INT:
        case OBJECT_FLOAT:
            return obj->data.n;
        case OBJECT_STR:
            return object_str_hash(obj);
    };
}

bool object_eq(object *a, object *b) {
    assert(object_hashable(a) && object_hashable(b));
    if (a->type != b->type) {
        return false;
    }
    switch (a->type) {
        case OBJECT_NONE:
            return true;
        case OBJECT_BOOL:
            return a == b;
        case OBJECT_INT:
            return a->data.n == b->data.n;
        case OBJECT_FLOAT:
            return a->data.f == b->data.f;
        case OBJECT_STR:
            return u_strcmp(a->data.str, b->data.str) == 0;
    };
}

void object_free(object *obj) {
    switch (obj->type) {
        case OBJECT_NONE:
        case OBJECT_BOOL:
            return;
        case OBJECT_INT:
        case OBJECT_FLOAT:
            free(obj);
            return;
        case OBJECT_STR:
            free(obj->data.str);
            free(obj);
            return;
        case OBJECT_LIST:
            while (object_list_length(obj)) {
                object_list_remove(obj, 0);
            }
            free(obj);
            return;
        case OBJECT_MAP:
            map_clear(&obj->data.m);
            free(obj->data.m.data);
            free(obj);
            return;
    };
}

object *object_copy(object *obj) {
    switch (obj->type) {
        case OBJECT_BOOL:
        case OBJECT_NONE:
            return obj;
        case OBJECT_INT:
            return object_int(obj->data.n);
        case OBJECT_FLOAT:
            return object_float(obj->data.f);
        case OBJECT_STR:
            return object_str(obj->data.str);
        case OBJECT_LIST:
            return object_list_copy(obj);
        case OBJECT_MAP:
            return object_map_copy(obj);
    };
}

bool object_bool_get(object *obj) {
    assert(obj->type == OBJECT_BOOL);
    if (obj == object_true) {
        return true;
    }
    return false;
}

int64_t object_int_get(object *obj) {
    assert(obj->type == OBJECT_INT);
    return obj->data.n;
}

double object_float_get(object *obj) {
    assert(obj->type == OBJECT_FLOAT);
    return obj->data.f;
}

UChar *object_str_get(object *obj) {
    assert(obj->type == OBJECT_STR);
    return u_strdup(obj->data.str);
}

void object_list_set(object *obj, int32_t i, object *value) {
    assert(obj->type == OBJECT_LIST);
    list_set(&obj->data.l, i, value);
}

void object_list_insert_at(object *obj, int32_t i, object *value) {
    assert(obj->type == OBJECT_LIST);
    list_insert_at(&obj->data.l, i, value);
}

void object_list_remove(object *obj, int32_t i) {
    assert(obj->type == OBJECT_LIST);
    list_remove(&obj->data.l, i);
}

object *object_list_get(object *obj, int32_t i) {
    assert(obj->type == OBJECT_LIST);
    return list_get(&obj->data.l, i);
}

int32_t object_list_length(object *obj) {
    return list_length(&obj->data.l);
}

void object_map_set(object *obj, object *key, object *val) {
    assert(obj->type == OBJECT_MAP);
    assert(object_hashable(key));
    map_set(&obj->data.m, key, val);
}

object *object_map_get(object *obj, object *key) {
    assert(obj->type == OBJECT_MAP);
    assert(object_hashable(key));
    return map_get(&obj->data.m, key);
}
