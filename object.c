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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "object.h"
#include "list.h"
#include "map.h"

struct object {
    unsigned char type;
    unsigned int ref;
    union {
        finger_branch l;
        map m;
        int64_t n;
        double f;
        char_t *str;
        unsigned char b;
    } data;
};

struct object_iterator {
    object *dst;
    uint32_t pos;
};

unsigned char none_type = OBJECT_NONE;

#define none ((object *)&none_type)

object bool_true = {OBJECT_BOOL, 0, { .b = true }};
object bool_false = {OBJECT_BOOL, 0, { .b = false }};

#define object_true (&bool_true)
#define object_false (&bool_false)

object *object_map() {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_MAP;
    obj->ref = 1;
    map_init(&obj->data.m, 16);
    return obj;
}

object *object_list() {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_LIST;
    obj->ref = 1;
    obj->data.l.left = NULL;
    obj->data.l.right = NULL;
    obj->data.l.nodes = 0;
    return obj;
}

object *object_str(char_t *str) {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_STR;
    obj->ref = 1;
    obj->data.str = str_strdup(str);
    return obj;
}

object *object_int(int64_t n) {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_INT;
    obj->ref = 1;
    obj->data.n = n;
    return obj;
}

object *object_float(double f) {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_FLOAT;
    obj->ref = 1;
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
    object *res = malloc(sizeof(object));
    res->type = OBJECT_LIST;
    res->ref = 1;
    res->data.l.right = list_copy(obj->data.l.right);
    res->data.l.left = list_copy(obj->data.l.left);
    res->data.l.nodes = obj->data.l.nodes;
    return res;
}

static object *object_map_copy(object *obj) {
    object *res = malloc(sizeof(object));
    res->type = OBJECT_MAP;
    res->ref = 1;
    
    map_copy(&obj->data.m, &res->data.m);
    
    return res;
}

int object_type(object *obj) {
    return obj->type;
}

bool object_hashable(object *obj) {
    return !(obj->type == OBJECT_LIST ||
             obj->type == OBJECT_MAP ||
             obj->type == OBJECT_FLOAT);
}

bool object_iterable(object *obj) {
    return (obj->type == OBJECT_LIST ||
            obj->type == OBJECT_MAP);
}

static uint32_t object_str_hash(object *obj) {
    uint32_t out = 5381;
    size_t len = str_strlen(obj->data.str);
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
        case OBJECT_STR:
            return str_strcmp(a->data.str, b->data.str) == 0;
    };
}

static bool dec_ref(object *obj) {
    return --obj->ref == 0;
}

void object_free(object *obj) {
    switch (obj->type) {
        case OBJECT_NONE:
        case OBJECT_BOOL:
            return;
        case OBJECT_INT:
        case OBJECT_FLOAT:
            if (dec_ref(obj)) {
                free(obj);
            }
            return;
        case OBJECT_STR:
            if (dec_ref(obj)) {
                free(obj->data.str);
                free(obj);
            }
            return;
        case OBJECT_LIST:
            if (dec_ref(obj)) {
                while (object_list_length(obj)) {
                    object_list_remove(obj, 0);
                }
                free(obj);
            }
            return;
        case OBJECT_MAP:
            if (dec_ref(obj)) {
                map_clear(&obj->data.m);
                free(obj->data.m.data);
                free(obj);
            }
            return;
    };
}

object *object_copy(object *obj) {
    switch (obj->type) {
        case OBJECT_BOOL:
        case OBJECT_NONE:
            return obj;
        case OBJECT_INT:
        case OBJECT_FLOAT:
        case OBJECT_STR:
            ++obj->ref;
            return obj;
        case OBJECT_LIST:
            return object_list_copy(obj);
        case OBJECT_MAP:
            return object_map_copy(obj);
    };
}

bool object_bool_get(object *obj) {
    assert(obj->type == OBJECT_BOOL);
    return obj->data.b;
}

int64_t object_int_get(object *obj) {
    assert(obj->type == OBJECT_INT);
    return obj->data.n;
}

double object_float_get(object *obj) {
    assert(obj->type == OBJECT_FLOAT);
    return obj->data.f;
}

char_t *object_str_get(object *obj) {
    assert(obj->type == OBJECT_STR);
    return str_strdup(obj->data.str);
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

static void object_iterator_map_jmpnext(object_iterator *it) {
    ++it->pos;
    while (it->pos < it->dst->data.m.sz) {
        if (it->dst->data.m.data[it->pos].key != NULL) {
            break;
        }
        ++it->pos;
    }
}

object_iterator *object_iterate(object *obj) {
    assert(object_iterable(obj));
    object_iterator *it = malloc(sizeof(object_iterator));
    it->dst = obj;
    if (obj->type == OBJECT_MAP) {
        it->pos = -1;
        object_iterator_map_jmpnext(it);
    } else {
        it->pos = 0;
    }
    return it;
}

void object_iterator_free(object_iterator *it) {
    free(it);
}

bool object_iterator_hasnext(object_iterator *it) {
    if (it->dst->type == OBJECT_MAP) {
        return it->pos < it->dst->data.m.sz - 1;
    } else if (it->dst->type == OBJECT_LIST) {
        return it->pos < object_list_length(it->dst);
    }
}

object *object_iterator_getnext(object_iterator *it) {
    assert(object_iterator_hasnext(it));
    
    if (it->dst->type == OBJECT_MAP) {
        object *ret = object_list();
        record *rec = &it->dst->data.m.data[it->pos];
        object_list_set(ret, 0, rec->key);
        object_list_set(ret, 1, rec->val);
        object_iterator_map_jmpnext(it);
        return ret;
    }
    
    if (it->dst->type == OBJECT_LIST) {
        return object_list_get(it->dst, (it->pos)++);
    }
}

static size_t object_join_sz(object *obj) {
    assert(obj->type == OBJECT_LIST || obj->type == OBJECT_STR);
    if (obj->type == OBJECT_STR) {
        return str_strlen(obj->data.str);
    } else {
        size_t len = 0;
        int32_t i;
        for (i = 0; i < object_list_length(obj); ++i) {
            object *item = object_list_get(obj, i);
            len += object_join_sz(item);
            object_free(item);
        }
        return len;
    }
}

static void object_join_write(object *obj, char_t *str) {
    assert(obj->type == OBJECT_LIST || obj->type == OBJECT_STR);
    if (obj->type == OBJECT_STR) {
        str_strcpy(str, obj->data.str);
    } else {
        size_t i;
        for (i = 0; i < object_list_length(obj); ++i) {
            object *item = object_list_get(obj, i);
            object_join_write(item, str);
            object_free(item);
            str += object_join_sz(item);
        }
    }
}

char_t *object_join(object *obj) {
    assert(obj->type == OBJECT_LIST || obj->type == OBJECT_STR);
    size_t len = object_join_sz(obj);
    char_t *res = malloc(sizeof(char_t) * (len + 1));
    object_join_write(obj, res);
    return res;
}

static uint32_t object_to_json_len(object *, bool);
static uint32_t object_to_json_write(char_t *, object *, bool);

static uint32_t int_to_json_len(int64_t i) {
    if (i == 0) {
        return 1;
    }
    int64_t n = i;
    if (n < 0) {
        n *= -1;
    }
    uint32_t len = ceil(log10(n + 1));
    if (i < 0) {
        len += 1;
    }
    return len;
}

static uint32_t int_to_json_write(char_t *str, int64_t i) {
    if (i == 0) {
        *str = '0';
        return 1;
    }
    int64_t j = i;
    if (j < 0) {
        j *= -1;
    }
    uint32_t len = ceil(log10(j + 1));
    uint32_t n = len;
    if (i < 0) {
        *str = '-';
        str += 1;
        len += 1;
        i *= -1;
    }
    while (i) {
        --n;
        str[n] = (i%10) + '0';
        i -= i%10;
        i /= 10;
    }
    return len;
}

static uint32_t str_to_json_len(object *obj) {
    char_t *str = object_str_get(obj);
    uint32_t sz = str_strlen(str);
    uint32_t len = 2;
    uint32_t i = 0;
    while (i < sz) {
        uint32_t c = str_next(str, &i, sz);
        if (c == '\\' || c == '\n' || c == '"' || c == '\t') {
            len += 2;
        } else {
            len += str_encoding_length(c);
        }
    }
    free(str);
    return len;
}

static uint32_t str_to_json_write(char_t *str, object *obj) {
    uint32_t i = 0;
    uint32_t j = 0;
    str_append(str, &j, '"');
    char_t *val = object_str_get(obj);
    uint32_t sz = str_strlen(val);
    while (i < sz) {
        uint32_t c = str_next(val, &i, sz);
        if (c == '\\' || c == '\n' || c == '"' || c == '\t') {
            str_append(str, &j, '\\');
            switch (c) {
                case '\\':
                    str_append(str, &j, '\\');
                    break;
                case '\n':
                    str_append(str, &j, 'n');
                    break;
                case '"':
                    str_append(str, &j, '"');
                    break;
                case '\t':
                    str_append(str, &j, 't');
                    break;
            };
        } else {
            str_append(str, &j, c);
        }
    }
    str_append(str, &j, '"');
    free(val);
    return j;
}

static uint32_t float_to_json_len(double val) {
    assert(!isnan(val) && !isinf(val));
    int sz = snprintf(NULL, 0, "%.17g", val);
    assert(sz > 0);
    return sz;
}

static uint32_t float_to_json_write(char_t *str, double val) {
    assert(!isnan(val) && !isinf(val));
    char *out;
    int sz = asprintf(&out, "%.17g", val);
    str_convert(out, str, sz + 1);
    free(out);
    return sz;
}

static uint32_t list_to_json_len(object *obj, bool pretty) {
    assert(pretty == false && "pretty printing hasn't been written");
    uint32_t len = 2; /* opening and closing brackets */
    object_iterator *it = object_iterate(obj);
    object *item;
    
    while (object_iterator_hasnext(it)) {
        item = object_iterator_getnext(it);
        
        len += object_to_json_len(item, pretty);
        
        if (object_iterator_hasnext(it)) {
            len += 1; /* comma */
        }
        
        object_free(item);
    }
    
    object_iterator_free(it);
    return len;
}

static uint32_t list_to_json_write(char_t *str, object *obj, bool pretty) {
    assert(pretty == false && "pretty printing hasn't been written");
    uint32_t i = 0;
    str_append(str, &i, '[');
    object_iterator *it = object_iterate(obj);
    object *item;
    
    while (object_iterator_hasnext(it)) {
        item = object_iterator_getnext(it);
        
        i += object_to_json_write(str + i, item, pretty);
        
        if (object_iterator_hasnext(it)) {
            str_append(str, &i, ',');
        }
        
        object_free(item);
    }
    
    object_iterator_free(it);
    str_append(str, &i, ']');
    return i;
}

static uint32_t map_to_json_len(object *obj, bool pretty) {
    assert(pretty == false && "pretty printing hasn't been written");
    uint32_t len = 2;
    
    object_iterator *it = object_iterate(obj);
    object *item;
    
    while (object_iterator_hasnext(it)) {
        item = object_iterator_getnext(it);
        
        object *key = object_list_get(item, 0);
        object *val = object_list_get(item, 1);
        
        len += object_to_json_len(key, pretty);
        len += 1;
        len += object_to_json_len(val, pretty);
        
        if (object_iterator_hasnext(it)) {
            len += 1;
        }
        
        object_free(key);
        object_free(val);
        object_free(item);
    }
    
    object_iterator_free(it);
    return len;
}

static uint32_t map_to_json_write(char_t *str, object *obj, bool pretty) {
    assert(pretty == false && "pretty printing hasn't been written");
    uint32_t i = 0;
    
    str_append(str, &i, '{');
    
    object_iterator *it = object_iterate(obj);
    object *item;
    
    while (object_iterator_hasnext(it)) {
        item = object_iterator_getnext(it);
        
        object *key = object_list_get(item, 0);
        object *val = object_list_get(item, 1);
        
        i += object_to_json_write(str + i, key, pretty);
        str_append(str, &i, ':');
        i += object_to_json_write(str + i, val, pretty);
        
        if (object_iterator_hasnext(it)) {
            str_append(str, &i, ',');
        }
        
        object_free(key);
        object_free(val);
        object_free(item);
    }
    
    object_iterator_free(it);
    str_append(str, &i, '}');
    return i;
}

static uint32_t object_to_json_len(object *obj, bool pretty) {
    switch (obj->type) {
        case OBJECT_NONE:
            return 4;
        case OBJECT_BOOL:
            if (object_bool_get(obj)) {
                return 4;
            } else {
                return 5;
            }
        case OBJECT_INT:
            return int_to_json_len(object_int_get(obj));
        case OBJECT_STR:
            return str_to_json_len(obj);
        case OBJECT_FLOAT:
            return float_to_json_len(object_float_get(obj));
        case OBJECT_LIST:
            return list_to_json_len(obj, pretty);
        case OBJECT_MAP:
            return map_to_json_len(obj, pretty);
    };
}

static uint32_t object_to_json_write(char_t *str, object *obj, bool pr) {
    STR_INIT(none_string, "null", 4);
    STR_INIT(true_string, "true", 4);
    STR_INIT(false_string, "false", 5);
    switch (obj->type) {
        case OBJECT_NONE:
            str_strcpy(str, none_string);
            return 4;
        case OBJECT_BOOL:
            if (object_bool_get(obj)) {
                str_strcpy(str, true_string);
                return 4;
            } else {
                str_strcpy(str, false_string);
                return 5;
            }
        case OBJECT_INT:
            return int_to_json_write(str, object_int_get(obj));
        case OBJECT_FLOAT:
            return float_to_json_write(str, object_float_get(obj));
        case OBJECT_STR:
            return str_to_json_write(str, obj);
        case OBJECT_LIST:
            return list_to_json_write(str, obj, pr);
        case OBJECT_MAP:
            return map_to_json_write(str, obj, pr);
    };
}

char_t *object_to_json(object *obj, bool pretty) {
    uint32_t n = object_to_json_len(obj, pretty);
    char_t *res = malloc(sizeof(char_t) * (n + 1));
    object_to_json_write(res, obj, pretty);
    res[n] = '\0';
    return res;
}

static uint32_t get_after_ws(const char_t *str, uint32_t *i, uint32_t sz) {
    uint32_t c = ' ';
    while ((c == ' ' || c == '\t' || c == '\r' || c == '\n') && (*i < sz)) {
        c = str_next(str, i, sz);
    }
    return c;
}

typedef struct {
    object *obj;
    uint32_t i;
} parse_result;

parse_result object_from_json_int(const char_t *str, uint32_t);

static parse_result parse_string(uint32_t i, uint32_t sz, const char_t *str) {
    uint32_t n = 0, m = 0;
    uint32_t start = i;
    uint32_t c;
    int stage;
    char_t *string;
    for (stage = 1; stage < 3; ++stage) {
        i = start;
        while (1) {
            if (i >= sz) {
                break;
            }
            c = str_next(str, &i, sz);
            if (c == '\\') {
                if (i >= sz) {
                    parse_result res = {NULL, i};
                    return res;
                }
                c = str_next(str, &i, sz);
                if ((c != '"') &&
                    (c != '\\') &&
                    (c != '/') &&
                    (c != 'b') &&
                    (c != 'f') &&
                    (c != 'n') &&
                    (c != 'r') &&
                    (c != 't') &&
                    (c != 'u')) {
                        parse_result res = {NULL, i};
                        return res;
                }
                if (c == 'u') {
                    int j;
                    uint32_t construct = 0;
                    for (j = 0; j < 4; ++j) {
                        if (i >= sz) {
                            parse_result res = {NULL, i};
                            return res;
                        }
                        c = str_next(str, &i, sz);
                        if ((c >= 'a' && c <= 'f') ||
                            (c >= 'A' && c <= 'F') ||
                            (c >= '0' && c <= '9')) {
                                if (c >= 'a' && c <= 'f') {
                                    c = c - 'a' + 10;
                                } else if (c >= 'A' && c <= 'F') {
                                    c = c - 'A' + 10;
                                } else {
                                    c = c - '0';
                                }
                                construct *= 16;
                                construct += c;
                        } else {
                            parse_result res = {NULL, i};
                            return res;
                        }
                    }
                    c = construct;
                    if (c == '\0') {
                        parse_result res = {NULL, i};
                        return res;
                    }
                } else if (c == '"') {
                    c = '"';
                } else if (c == '\\') {
                    c = '\\';
                } else if (c == '/') {
                    c = '/';
                } else if (c == 'b') {
                    c = '\b';
                } else if (c == 'f') {
                    c = '\f';
                } else if (c == 'n') {
                    c = '\n';
                } else if (c == 'r') {
                    c = '\r';
                } else if (c == 't') {
                    c = '\t';
                }
            } else if (c == '"') {
                break;
            }
            if (stage == 1) {
                n += str_encoding_length(c);
            } else {
                str_append(string, &m, c);
            }
        }
        if (stage == 1) {
            string = malloc(sizeof(char_t) * (n + 1));
        } else {
            string[m] = 0;
        }
    }
    parse_result res = {object_str(string), i};
    free(string);
    return res;
}

static parse_result parse_map(uint32_t i, uint32_t sz, const char_t *str) {
    object *m = object_map();
    if (i >= sz) {
        object_free(m);
        parse_result res = {NULL, i};
        return res;
    }
    uint32_t c = get_after_ws(str, &i, sz);
    if (c == '}') {
        parse_result res = {m, i};
        return res;
    }
    while (1) {
        if (c != '"') {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
        parse_result key = parse_string(i, sz, str);
        i = key.i;
        if (key.obj == NULL || i >= sz) {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
        c = get_after_ws(str, &i, sz);
        if (c != ':') {
            object_free(m);
            object_free(key.obj);
            parse_result res = {NULL, i};
            return res;
        }
        parse_result val = object_from_json_int(str + i, sz - 1);
        i += val.i;
        if (val.obj == NULL) {
            object_free(m);
            object_free(key.obj);
            parse_result res = {NULL, i};
            return res;
        }
        object_map_set(m, key.obj, val.obj);
        object_free(key.obj);
        object_free(val.obj);
        if (i >= sz) {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
        c = get_after_ws(str, &i, sz);
        if (c == '}') {
            break;
        }
        if (c != ',' || i >= sz) {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
        c = get_after_ws(str, &i, sz);
    }
    parse_result res = {m, i};
    return res;
}

static parse_result parse_list(uint32_t i, uint32_t sz, const char_t *str) {
    object *lst = object_list();
    uint32_t next = i;
    if (i >= sz) {
        object_free(lst);
        parse_result res = {NULL, i};
        return res;
    }
    uint32_t c = get_after_ws(str, &next, sz);
    if (c == ']') {
        parse_result res = {lst, next};
        return res;
    }
    while (1) {
        parse_result item = object_from_json_int(str + i, sz - i);
        i += item.i;
        if (item.obj == NULL) {
            object_free(lst);
            parse_result res = {NULL, i};
            return res;
        }
        object_list_set(lst, object_list_length(lst), item.obj);
        object_free(item.obj);
        
        if (i >= sz) {
            object_free(lst);
            parse_result res = {NULL, i};
            return res;
        }
        c = get_after_ws(str, &i, sz);
        
        if (c == ']') {
            break;
        }
        if (c != ',') {
            object_free(lst);
            parse_result res = {NULL, i};
            return res;
        }
    }
    parse_result res = {lst, i};
    return res;
}

static parse_result parse_num
        (uint32_t c, uint32_t i, uint32_t sz, const char_t *str) {
    int64_t out = 0;
    bool negative = false;
    if (c == '-') {
        negative = true;
        if (i >= sz) {
            parse_result res = {NULL, i};
            return res;
        }
        c = str_next(str, &i, sz);
    }
    if (c != '0') {
        while (1) {
            if (c >= '0' && c <= '9') {
                out *= 10;
                out += c - '0';
            } else {
                break;
            }
            if (i >= sz) {
                break;
            }
            c = str_next(str, &i, sz);
        }
    } else {
        if (i >= sz) {
            parse_result res = {object_int(out), i};
            return res;
        }
        c = str_next(str, &i, sz);
    }
    if (c != '.' && c != 'e' && c != 'E') {
        if (negative) {
            out *= -1;
        }
        parse_result res = {object_int(out), i - 1};
        return res;
    }
    double d = out;
    if (c == '.') {
        if (i >= sz) {
            parse_result res = {NULL, i};
            return res;
        }
        double mul = 0.1;
        while (1) {
            c = str_next(str, &i, sz);
            if (c >= '0' && c <= '9') {
                d += ((double)(c - '0'))*mul;
                mul *= 0.1;
            } else {
                break;
            }
            if (i >= sz) {
                break;
            }
        }
        if (mul >= 0.09) {
            parse_result res = {NULL, i};
            return res;
        }
    }
    if (c == 'e' || c == 'E') {
        int32_t e = 0;
        if (i >= sz) {
            parse_result res = {NULL, i};
            return res;
        }
        c = str_next(str, &i, sz);
        bool e_negative = false;
        if (c == '-') {
            e_negative = true;
        }
        if (c == '-' || c == '+') {
            if (i >= sz) {
                parse_result res = {NULL, i};
                return res;
            }
            c = str_next(str, &i, sz);
        }
        bool e_num = false;
        while (1) {
            if (c >= '0' && c <= '9') {
                e *= 10;
                e += c - '0';
                e_num = true;
            } else {
                break;
            }
            if (i >= sz) {
                break;
            }
            c = str_next(str, &i, sz);
        }
        if (!e_num) {
            parse_result res = {NULL, i};
            return res;
        }
        if (e_negative) {
            e *= -1;
        }
        d *= pow(10, e);
    }
    if (negative) {
        d *= -1;
    }
    parse_result res = {object_float(d), i-1};
    return res;
}

parse_result object_from_json_int(const char_t *str, uint32_t sz) {
    uint32_t i=0;
    if (i >= sz) {
        parse_result res = {NULL, i};
        return res;
    }
    uint32_t c = get_after_ws(str, &i, sz);
    if (c == '{') {
        return parse_map(i, sz, str);
    } else if (c == '[') {
        return parse_list(i, sz, str);
    } else if (c == 'n') {
        STR_INIT(null, "ull", 3);
        if (str_memcmp(str + i, null, 3) == 0) {
            parse_result res = {object_none(), i + 3};
            return res;
        }
        parse_result res = {NULL, i};
        return res;
    } else if (c == 't') {
        STR_INIT(true_string, "rue", 3);
        if (str_memcmp(str + i, true_string, 3) == 0) {
            parse_result res = {object_bool(true), i + 3};
            return res;
        }
        parse_result res = {NULL, i};
        return res;
    } else if (c == 'f') {
        STR_INIT(false_string, "alse", 4);
        if (str_memcmp(str + i, false_string, 4) == 0) {
            parse_result res = {object_bool(false), i + 4};
            return res;
        }
        parse_result res = {NULL, i};
        return res;
    } else if (c == '"') {
        return parse_string(i, sz, str);
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        return parse_num(c, i, sz, str);
    } else {
        parse_result res = {NULL, i};
        return res;
    }
}

object *object_from_json(const char_t *str) {
    parse_result res = object_from_json_int(str, str_strlen(str));
    return res.obj;
}
