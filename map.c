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

#include "stdlib.h"

#include "map.h"

#define HASH_TRIES 5

static uint32_t hash(uint32_t a){
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}

static void map_resize(map *m, uint32_t sz) {
    map new;
    map_init(&new, sz);
    uint32_t i;
    for (i = 0; i < m->sz; ++i) {
        record *rec = &new.data[i];
        rec->key = NULL;
        rec->val = NULL;
    }
    for (i = 0; i < m->sz; ++i) {
        record *rec = &m->data[i];
        if (rec->key != NULL) {
            map_set(&new, rec->key, rec->val);
        }
    }
    map_clear(m);
    *m = new;
}

static void map_upsize(map *m) {
    map_resize(m, m->sz * 2);
}

static void map_downsize(map *m) {
    if (m->sz / 2 >= 16) {
        map_resize(m, m->sz / 2);
    }
}

void map_copy(map *src, map *dst) {
    dst->sz = src->sz;
    dst->elems = src->elems;
    dst->data = malloc(sizeof(record) * src->sz);
    uint32_t i;
    for (i = 0; i < src->sz; ++i) {
        record *src_rec = &src->data[i];
        record *dst_rec = &dst->data[i];
        if (src_rec->key == NULL) {
            dst_rec->key = NULL;
            dst_rec->val = NULL;
        } else {
            dst_rec->key = object_copy(src_rec->key);
            dst_rec->val = object_copy(src_rec->val);
        }
    }
}

void map_init(map *m, uint32_t sz) {
    m->sz = sz;
    m->elems = 0;
    m->data = malloc(sizeof(record) * sz);
    uint32_t i;
    for (i = 0; i < sz; ++i) {
        m->data[i].key = NULL;
        m->data[i].val = NULL;
    }
}

void map_set(map *m, object *key, object *val) {
    uint32_t key_hash = object_hash(key);
    int i;
    for (i = 0; i < HASH_TRIES; ++i) {
        record *rec = &m->data[key_hash % m->sz];
        if (rec->key == NULL) {
            rec->key = object_copy(key);
            rec->val = object_copy(val);
            m->elems += 1;
            return;
        } else {
            if (object_eq(key, rec->key)) {
                object_free(rec->val);
                rec->val = object_copy(val);
                return;
            }
        }
        key_hash = hash(key_hash);
    }
    map_upsize(m);
    map_set(m, key, val);
}

object *map_get(map *m, object *key) {
    uint32_t key_hash = object_hash(key);
    int i;
    for (i = 0; i < HASH_TRIES; ++i) {
        record *rec = &m->data[key_hash % m->sz];
        if (object_eq(key, rec->key)) {
            return object_copy(rec->val);
        }
        key_hash = hash(key_hash);
    }
    return NULL;
}

void map_rem(map *m, object *key) {
    uint32_t key_hash = object_hash(key);
    int i;
    for (i = 0; i < HASH_TRIES; ++i) {
        record *rec = &m->data[key_hash % m->sz];
        if (object_eq(key, rec->key)) {
            object_free(rec->key);
            object_free(rec->val);
            rec->key = NULL;
            rec->val = NULL;
            m->elems -= 1;
            if (m->elems * 4 < m->sz) {
                map_downsize(m);
            }
        }
        key_hash = hash(key_hash);
    }
}

void map_clear(map *m) {
    uint32_t i;
    for (i = 0; i < m->sz; ++i) {
        record *rec = &m->data[i];
        if (rec->key != NULL) {
            object_free(rec->key);
            object_free(rec->val);
            rec->key = NULL;
            rec->val = NULL;
        }
    }
    m->elems = 0;
}

uint32_t map_length(map *m) {
    return m->elems;
}
