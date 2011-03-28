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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

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
        unsigned char b;
    } data;
};

unsigned char none_type = OBJECT_NONE;

#define none ((object *)&none_type)

object bool_true = {OBJECT_BOOL, { .b = true }};
object bool_false = {OBJECT_BOOL, { .b = false }};

#define object_true (&bool_true)
#define object_false (&bool_false)

object *object_map() {
    object *res = malloc(sizeof(object));
    res->type = OBJECT_MAP;
    map_init(&res->data.m, 16);
    return res;
}

object *object_list() {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_LIST;
    obj->data.l.left = NULL;
    obj->data.l.right = NULL;
    obj->data.l.nodes = 0;
    return obj;
}

object *object_str(UChar *str) {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_STR;
    obj->data.str = u_strdup(str);
    return obj;
}

object *object_int(int64_t n) {
    object *obj = malloc(sizeof(object));
    obj->type = OBJECT_INT;
    obj->data.n = n;
    return obj;
}

object *object_float(double f) {
    object *obj = malloc(sizeof(object));
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
    object *res = malloc(sizeof(object));
    res->type = OBJECT_LIST;
    res->data.l.right = list_copy(obj->data.l.right);
    res->data.l.left = list_copy(obj->data.l.left);
    res->data.l.nodes = obj->data.l.nodes;
    return res;
}

static object *object_map_copy(object *obj) {
    object *res = malloc(sizeof(object));
    res->type = OBJECT_MAP;
    
    map_copy(&obj->data.m, &res->data.m);
    
    return res;
}

int object_type(object *obj) {
    return obj->type;
}

bool object_hashable(object *obj) {
    if (obj->type == OBJECT_LIST ||
        obj->type == OBJECT_MAP ||
        obj->type == OBJECT_FLOAT) {
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

static size_t object_join_sz(object *obj) {
    assert(obj->type == OBJECT_LIST || obj->type == OBJECT_STR);
    if (obj->type == OBJECT_STR) {
        return u_strlen(obj->data.str);
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

static void object_join_write(object *obj, UChar *str) {
    assert(obj->type == OBJECT_LIST || obj->type == OBJECT_STR);
    if (obj->type == OBJECT_STR) {
        u_strcpy(str, obj->data.str);
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

UChar *object_join(object *obj) {
    assert(obj->type == OBJECT_LIST || obj->type == OBJECT_STR);
    size_t len = object_join_sz(obj);
    UChar *res = malloc(sizeof(UChar) * len + 1);
    object_join_write(obj, res);
    return res;
}

object *object_to_json_joinable(object *obj, bool pretty) {
    U_STRING(none_string, "null", 4);
    U_STRING(true_string, "true", 4);
    U_STRING(false_string, "false", 5);
    switch (obj->type) {
        case OBJECT_NONE:
            return object_str(none_string);
        case OBJECT_BOOL:
            if (obj->data.b) {
                return object_str(true_string);
            } else {
                return object_str(false_string);
            }
        case OBJECT_INT:
            break;
        case OBJECT_STR:
            break;
        case OBJECT_FLOAT:
            break;
        case OBJECT_LIST:
            break;
        case OBJECT_MAP:
            break;
    };
}

UChar *object_to_json(object *obj, bool pretty) {
    object *joinable = object_to_json_joinable(obj, pretty);
    UChar *out = object_join(joinable);
    object_free(joinable);
    return out;
}

static UChar32 get_next(UChar *str, uint32_t *i, uint32_t *sz) {
    UChar32 c;
    U16_NEXT(str, *i, *sz, c);
    return c;
}

static UChar32 get_after_ws(UChar *str, uint32_t *i, uint32_t *sz) {
    UChar32 c = ' ';
    while ((c == ' ' || c == '\t' || c == '\r' || c == '\n') && (*i < *sz)) {
        c = get_next(str, i, sz);
    }
    return c;
}

typedef struct {
    object *obj;
    uint32_t i;
} parse_result;

parse_result object_from_json_int(UChar *str);

static parse_result parse_string(uint32_t i, uint32_t sz, UChar *str) {
    uint32_t n = 0, m = 0;
    uint32_t start = i;
    UChar32 c;
    int stage;
    UChar *string;
    for (stage = 1; stage < 3; ++stage) {
        i = start;
        while (1) {
            if (i >= sz) {
                break;
            }
            c = get_next(str, &i, &sz);
            if (c == '\\') {
                if (i >= sz) {
                    parse_result res = {NULL, i};
                    return res;
                }
                c = get_next(str, &i, &sz);
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
                    UChar32 construct = 0;
                    for (j = 0; j < 4; ++j) {
                        if (i >= sz) {
                            parse_result res = {NULL, i};
                            return res;
                        }
                        c = get_next(str, &i, &sz);
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
                n += U16_LENGTH(c);
            } else {
                U16_APPEND_UNSAFE(string, m, c);
            }
        }
        if (stage == 1) {
            string = malloc(sizeof(UChar) * (n + 1));
        } else {
            string[m] = 0;
        }
    }
    parse_result res = {object_str(string), i};
    free(string);
    return res;
}

static parse_result parse_map(uint32_t i, uint32_t sz, UChar *str) {
    object *m = object_map();
    uint32_t next = i;
    if (i >= sz) {
        parse_result res = {NULL, i};
        return res;
    }
    UChar32 c = get_after_ws(str, &next, &sz);
    if (c == '}') {
        parse_result res = {m, next};
        return res;
    }
    while (1) {
        if (c != '"') {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
        parse_result key = parse_string(next, sz, str);
        i = key.i;
        if (key.obj == NULL || i >= sz) {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
        c = get_after_ws(str, &i, &sz);
        if (c != ':') {
            object_free(m);
            object_free(key.obj);
            parse_result res = {NULL, i};
            return res;
        }
        parse_result val = object_from_json_int(str + i);
        if (val.obj == NULL) {
            object_free(m);
            object_free(key.obj);
            parse_result res = {NULL, i + val.i};
            return res;
        }
        object_map_set(m, key.obj, val.obj);
        object_free(key.obj);
        object_free(val.obj);
        i += val.i;
        c = get_after_ws(str, &i, &sz);
        if (c == '}') {
            break;
        }
        if (c != ',') {
            object_free(m);
            parse_result res = {NULL, i};
            return res;
        }
    }
    parse_result res = {m, i};
    return res;
}

static parse_result parse_list(uint32_t i, uint32_t sz, UChar *str) {
    object *lst = object_list();
    uint32_t next = i;
    UChar32 c = get_after_ws(str, &next, &sz);
    if (c == ']') {
        parse_result res = {lst, next};
        return res;
    }
    while (1) {
        parse_result item = object_from_json_int(str + i);
        i += item.i;
        if (item.obj == NULL) {
            object_free(lst);
            parse_result res = {NULL, i};
            return res;
        }
        object_list_set(lst, object_list_length(lst), item.obj);
        object_free(item.obj);
        
        c = get_after_ws(str, &i, &sz);
        
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

typedef struct {
    int64_t n;
    uint32_t i;
} int_result;

static int_result parse_int(uint32_t i, uint32_t sz, UChar *str) {
    
}

static parse_result parse_num(UChar32 c, uint32_t i, uint32_t sz, UChar *str) {
    int64_t out = 0;
    bool negative = false;
    bool has_number = false;
    if (c == '-') {
        negative = true;
    } else {
        out = c - '0';
        has_number = true;
    }
    if (c != '0') {
        while (1) {
            if (i >= sz) {
                break;
            }
            c = get_next(str, &i, &sz);
            if (c >= '0' && c <= '9') {
                out *= 10;
                out += c - '0';
                has_number = true;
            } else {
                break;
            }
        }
    }
    if (!has_number) {
        parse_result res = {NULL, i};
            return res;
    }
    if (c != '.' && c != 'e' && c != 'E') {
        if (negative) {
            out *= -1;
        }
        parse_result res = {object_int(out), i};
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
            c = get_next(str, &i, &sz);
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
        /* TODO: negative powers */
        bool e_num = false;
        while (1) {
            c = get_next(str, &i, &sz);
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
        }
        if (!e_num) {
            parse_result res = {NULL, i};
            return res;
        }
        d *= pow(10, e);
    }
    if (negative) {
        d *= -1;
    }
    parse_result res = {object_float(d), i};
    return res;
}

parse_result object_from_json_int(UChar *str) {
    uint32_t i=0, sz=u_strlen(str);
    if (i >= sz) {
        parse_result res = {NULL, i};
        return res;
    }
    UChar32 c = get_after_ws(str, &i, &sz);
    if (c == '{') {
        return parse_map(i, sz, str);
    } else if (c == '[') {
        return parse_list(i, sz, str);
    } else if (c == 'n') {
        U_STRING(null, "ull", 3);
        if (u_memcmp(str + i, null, 3) == 0) {
            parse_result res = {object_none(), i + 3};
            return res;
        }
        parse_result res = {NULL, i};
        return res;
    } else if (c == 't') {
        U_STRING(true_string, "rue", 3);
        if (u_memcmp(str + i, true_string, 3) == 0) {
            parse_result res = {object_bool(true), i + 3};
            return res;
        }
        parse_result res = {NULL, i};
        return res;
    } else if (c == 'f') {
        U_STRING(false_string, "alse", 4);
        if (u_memcmp(str + i, false_string, 4) == 0) {
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

object *object_from_json(UChar *str) {
    parse_result res = object_from_json_int(str);
    return res.obj;
}
