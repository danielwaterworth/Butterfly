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

#include "iterator_test.h"
#include "object.h"

START_TEST (iterate_map_1) {
    STR_INIT(map_string, "{\"0\": \"first\", \"1\": \"second\"}", 29);
    
    STR_INIT(fk_str, "0", 1);
    STR_INIT(sk_str, "1", 1);
    
    STR_INIT(fv_str, "first", 5);
    STR_INIT(sv_str, "second", 6);
    
    object *map = object_from_json(map_string);
    
    object *fk = object_str(fk_str);
    object *sk = object_str(sk_str);
    
    object *fv = object_str(fv_str);
    object *sv = object_str(sv_str);
    
    object *item, *key, *val;
    
    object_iterator *it = object_iterate(map);
    
    fail_unless(object_iterator_hasnext(it), NULL);
    
    item = object_iterator_getnext(it);
    fail_unless(item != NULL, NULL);
    
    key = object_list_get(item, 0);
    val = object_list_get(item, 1);
    object_free(item);
    
    fail_unless(object_eq(key, fk), NULL);
    object_free(fk);
    object_free(key);
    
    fail_unless(object_eq(val, fv), NULL);
    object_free(fv);
    object_free(val);
    
    fail_unless(object_iterator_hasnext(it), NULL);
    
    item = object_iterator_getnext(it);
    fail_unless(item != NULL, NULL);
    
    key = object_list_get(item, 0);
    val = object_list_get(item, 1);
    object_free(item);
    
    fail_unless(object_eq(key, sk), NULL);
    object_free(sk);
    object_free(key);
    
    fail_unless(object_eq(val, sv), NULL);
    object_free(sv);
    object_free(val);
    
    fail_unless(!object_iterator_hasnext(it), NULL);
    
    object_iterator_free(it);
    object_free(map);
} END_TEST

START_TEST (iterate_map_2) {
    STR_INIT(map_string, "{}", 2);
    object *m = object_from_json(map_string);
    object_iterator *it = object_iterate(m);
    fail_unless(!object_iterator_hasnext(it), NULL);
} END_TEST

START_TEST (iterate_list_1) {
    STR_INIT(list_string, "[0, 1]", 6);
    object *lst = object_from_json(list_string);
    
    object *fst = object_int(0);
    object *snd = object_int(1);
    
    object_iterator *it = object_iterate(lst);
    
    object *item;
    
    fail_unless(object_iterator_hasnext(it), NULL);
    
    item = object_iterator_getnext(it);
    fail_unless(object_eq(item, fst), NULL);
    object_free(fst);
    object_free(item);
    
    fail_unless(object_iterator_hasnext(it), NULL);
    
    item = object_iterator_getnext(it);
    fail_unless(object_eq(item, snd), NULL);
    object_free(snd);
    object_free(item);
    
    fail_unless(!object_iterator_hasnext(it), NULL);
    
    object_free(lst);
} END_TEST

START_TEST (iterate_list_2) {
    STR_INIT(lst_str, "[0,1,2]", 8);
    object *obj = object_from_json(lst_str);
    object_iterator *it = object_iterate(obj);
    
    while (object_iterator_hasnext(it)) {
        object *item = object_iterator_getnext(it);
        object_free(item);
    }
    
    object_iterator_free(it);
} END_TEST

START_TEST (iterate_list_3) {
    STR_INIT(lst_str, "[0,1,2]", 8);
    object *obj = object_from_json(lst_str);
    int i;
    for (i = 0; i < 3; ++i) {
        object_iterator *it = object_iterate(obj);
        
        while (object_iterator_hasnext(it)) {
            object *item = object_iterator_getnext(it);
            object_free(item);
        }
        
        object_iterator_free(it);
    }
} END_TEST

START_TEST (iterate_list_4) {
    STR_INIT(lst_string, "[]", 2);
    object *lst = object_from_json(lst_string);
    object_iterator *it = object_iterate(lst);
    fail_unless(!object_iterator_hasnext(it), NULL);
} END_TEST

TCase *iterator_test_case() {
    TCase *tc = tcase_create("iteration");
    tcase_add_test(tc, iterate_map_1);
    tcase_add_test(tc, iterate_map_2);
    tcase_add_test(tc, iterate_list_1);
    tcase_add_test(tc, iterate_list_2);
    tcase_add_test(tc, iterate_list_3);
    tcase_add_test(tc, iterate_list_4);
    return tc;
}
