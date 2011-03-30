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

#include "primitive_test.h"
#include "object.h"

START_TEST (none_test) {
    object *obj = object_none();
    fail_unless(object_type(obj) == OBJECT_NONE);
    object_free(obj);
} END_TEST

START_TEST (int_test) {
    object *obj = object_int(100);
    fail_unless(object_type(obj) == OBJECT_INT);
    fail_unless(object_int_get(obj) == 100);
    object_free(obj);
} END_TEST

START_TEST (bool_test) {
    object *tr = object_bool(true);
    object *fa = object_bool(false);
    fail_unless(object_type(tr) == OBJECT_BOOL);
    fail_unless(object_type(fa) == OBJECT_BOOL);
    fail_unless(object_bool_get(tr) == true);
    fail_unless(object_bool_get(fa) == false);
    object_free(tr);
    object_free(fa);
} END_TEST

START_TEST (float_test) {
    object *obj = object_float(1.0);
    fail_unless(object_type(obj) == OBJECT_FLOAT, NULL);
    fail_unless(object_float_get(obj) == 1.0, NULL);
    object_free(obj);
} END_TEST

START_TEST (str_test) {
    STR_INIT(a, "test", 4);
    object *obj = object_str(a);
    fail_unless(object_type(obj) == OBJECT_STR, NULL);
    char_t *str = object_str_get(obj);
    fail_unless(str_strcmp(str, a) == 0, NULL);
    free(str);
    object_free(obj);
} END_TEST

TCase *primitive_test_case() {
    TCase *tc = tcase_create("primitive");
    tcase_add_test(tc, none_test);
    tcase_add_test(tc, int_test);
    tcase_add_test(tc, bool_test);
    tcase_add_test(tc, float_test);
    tcase_add_test(tc, str_test);
    return tc;
}
