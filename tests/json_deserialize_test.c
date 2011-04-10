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

#include <math.h>
#include <stdlib.h>

#include "json_deserialize_test.h"
#include "object.h"

START_TEST (test_null) {
    STR_INIT(null_test, "null", 4);
    object *obj = object_from_json(null_test);
    fail_unless(obj != NULL, NULL);
    object *none = object_none();
    fail_unless(object_eq(obj, none), NULL);
    object_free(obj);
    object_free(none);
} END_TEST

START_TEST (test_whitespace) {
    STR_INIT(null_test, "  \t\nnull\n", 9);
    object *obj = object_from_json(null_test);
    fail_unless(obj != NULL, NULL);
    object *none = object_none();
    fail_unless(object_eq(obj, none), NULL);
    object_free(obj);
    object_free(none);
} END_TEST

START_TEST (test_true) {
    STR_INIT(true_test, "true", 4);
    object *obj = object_from_json(true_test);
    fail_unless(obj != NULL, NULL);
    object *object_true = object_bool(true);
    fail_unless(object_eq(obj, object_true), NULL);
    object_free(obj);
    object_free(object_true);
} END_TEST

START_TEST (test_false) {
    STR_INIT(false_test, "false", 5);
    object *obj = object_from_json(false_test);
    fail_unless(obj != NULL, NULL);
    object *object_false = object_bool(false);
    fail_unless(object_eq(obj, object_false), NULL);
    object_free(obj);
    object_free(object_false);
} END_TEST

START_TEST (test_list_1) {
    STR_INIT(list_test, "[null, null, null]", 18);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_type(obj) == OBJECT_LIST, NULL);
    fail_unless(object_list_length(obj) == 3, NULL);
    object *none = object_none();
    
    object *item = object_list_get(obj, 0);
    fail_unless(object_eq(item, none), NULL);
    object_free(item);
    
    item = object_list_get(obj, 1);
    fail_unless(object_eq(item, none), NULL);
    object_free(item);
    
    item = object_list_get(obj, 2);
    fail_unless(object_eq(item, none), NULL);
    object_free(item);
    
    object_free(none);
    object_free(obj);
} END_TEST

START_TEST (test_list_2) {
    STR_INIT(list_test, "[\"test\", null, null]", 20);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_type(obj) == OBJECT_LIST, NULL);
    fail_unless(object_list_length(obj) == 3, NULL);
    object *none = object_none();
    
    object *item = object_list_get(obj, 0);
    STR_INIT(val_str, "test", 4);
    object *val = object_str(val_str);
    fail_unless(object_eq(item, val), NULL);
    object_free(item);
    
    item = object_list_get(obj, 1);
    fail_unless(object_eq(item, none), NULL);
    object_free(item);
    
    item = object_list_get(obj, 2);
    fail_unless(object_eq(item, none), NULL);
    object_free(item);
    
    object_free(none);
    object_free(obj);
} END_TEST

START_TEST (test_list_3) {
    STR_INIT(list_test, "[1]", 3);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_list_4) {
    STR_INIT(list_test, "[1.5]", 5);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_list_5) {
    STR_INIT(list_test, "[1e1]", 5);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_list_6) {
    STR_INIT(list_test, "[1.5e1]", 7);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_list_7) {
    STR_INIT(list_test, "[0]", 3);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_list_8) {
    STR_INIT(list_test, "[0.1]", 5);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_list_9) {
    STR_INIT(list_test, "[[]]", 4);
    object *obj = object_from_json(list_test);
    fail_unless(obj != NULL, NULL);
} END_TEST

START_TEST (test_int) {
    STR_INIT(int_test, "105", 3);
    object *obj = object_from_json(int_test);
    fail_unless(obj != NULL, NULL);
    object *n = object_int(105);
    fail_unless(object_eq(obj, n), NULL);
    object_free(obj);
    object_free(n);
} END_TEST

START_TEST (test_float_1) {
    STR_INIT(float_test, "1.75", 4);
    object *obj = object_from_json(float_test);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_type(obj) == OBJECT_FLOAT, NULL);
    double f = object_float_get(obj);
    fail_unless(fabs(1.75 - f) < 1e-5, NULL);
    object_free(obj);
} END_TEST

START_TEST (test_float_2) {
    STR_INIT(float_test, "1.", 2);
    object *obj = object_from_json(float_test);
    fail_unless(obj == NULL, NULL);
} END_TEST

START_TEST (test_float_3) {
    STR_INIT(float_test, "1e", 2);
    object *obj = object_from_json(float_test);
    fail_unless(obj == NULL, NULL);
} END_TEST

START_TEST (test_float_4) {
    STR_INIT(float_test, "1e5", 3);
    object *obj = object_from_json(float_test);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_type(obj) == OBJECT_FLOAT, NULL);
    double f = object_float_get(obj);
    fail_unless(fabs(1e5 - f) < 1e-5, NULL);
    object_free(obj);
} END_TEST

START_TEST (test_float_5) {
    STR_INIT(float_test, "1e-2", 4);
    object *obj = object_from_json(float_test);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_type(obj) == OBJECT_FLOAT, NULL);
    double f = object_float_get(obj);
    fail_unless(fabs(1e-2 - f) < 1e-5, NULL);
    object_free(obj);
} END_TEST

START_TEST (test_string_1) {
    STR_INIT(str_test, "\"test\"", 6);
    object *obj = object_from_json(str_test);
    STR_INIT(test, "test", 4);
    char_t *res = object_str_get(obj);
    fail_unless(str_strcmp(res, test) == 0, NULL);
    free(res);
    object_free(obj);
} END_TEST

START_TEST (test_string_2) {
    STR_INIT(str_test, " \"test1\" ", 9);
    object *obj = object_from_json(str_test);
    STR_INIT(test, "test1", 5);
    char_t *res = object_str_get(obj);
    fail_unless(str_strcmp(res, test) == 0, NULL);
    free(res);
    object_free(obj);
} END_TEST

START_TEST (test_map_1) {
    STR_INIT(map_test, "{\"hello\":\"world\"}", 17);
    object *obj = object_from_json(map_test);
    object *m = object_map();
    STR_INIT(key_str, "hello", 5);
    STR_INIT(val_str, "world", 5);
    object *key = object_str(key_str);
    object *val = object_str(val_str);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_eq(val, object_map_get(obj, key)), NULL);
    object_free(obj);
} END_TEST

START_TEST (test_map_2) {
    STR_INIT(map_test, "{\"hello\":\"world\", \"something\":null}", 35);
    object *obj = object_from_json(map_test);
    object *m = object_map();
    STR_INIT(key_str, "hello", 5);
    STR_INIT(val_str, "world", 5);
    object *key = object_str(key_str);
    object *val = object_str(val_str);
    fail_unless(obj != NULL, NULL);
    fail_unless(object_eq(val, object_map_get(obj, key)), NULL);
    object_free(obj);
} END_TEST

START_TEST (test_map_3) {
    STR_INIT(big_str, "{\"hello\":{\"something\":\"yeah\"}, \"num\":1.1}", 41);
    object *obj = object_from_json(big_str);
    fail_unless(obj != NULL, NULL);
} END_TEST

TCase *json_deserialize_test_case() {
    TCase *tc = tcase_create("json_deserialization");
    tcase_add_test(tc, test_null);
    tcase_add_test(tc, test_true);
    tcase_add_test(tc, test_false);
    tcase_add_test(tc, test_whitespace);
    tcase_add_test(tc, test_list_1);
    tcase_add_test(tc, test_list_2);
    tcase_add_test(tc, test_list_3);
    tcase_add_test(tc, test_list_4);
    tcase_add_test(tc, test_list_5);
    tcase_add_test(tc, test_list_6);
    tcase_add_test(tc, test_list_7);
    tcase_add_test(tc, test_list_8);
    tcase_add_test(tc, test_list_9);
    tcase_add_test(tc, test_int);
    tcase_add_test(tc, test_float_1);
    tcase_add_test(tc, test_float_2);
    tcase_add_test(tc, test_float_3);
    tcase_add_test(tc, test_float_4);
    tcase_add_test(tc, test_float_5);
    tcase_add_test(tc, test_string_1);
    tcase_add_test(tc, test_string_2);
    tcase_add_test(tc, test_map_1);
    tcase_add_test(tc, test_map_2);
    tcase_add_test(tc, test_map_3);
    return tc;
}
