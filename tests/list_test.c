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

#include "stdio.h"

#include "list_test.h"
#include "object.h"

START_TEST (test_1) {
    object *obj = object_list();
    fail_unless(object_list_length(obj) == 0, NULL);
    
    object *val = object_int(0);
    object_list_set(obj, 0, val);
    object_free(val);
    fail_unless(object_list_length(obj) == 1, NULL);
    
    val = object_int(1);
    object_list_set(obj, 1, val);
    object_free(val);
    fail_unless(object_list_length(obj) == 2, NULL);
    
    val = object_int(2);
    object_list_set(obj, 2, val);
    object_free(val);
    fail_unless(object_list_length(obj) == 3, NULL);
    
    val = object_int(3);
    object_list_set(obj, 3, val);
    object_free(val);
    fail_unless(object_list_length(obj) == 4, NULL);
    
    val = object_list_get(obj, 0);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 0);
    object_free(val);
    
    val = object_list_get(obj, 1);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 1, NULL);
    object_free(val);
    
    val = object_list_get(obj, 2);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 2, NULL);
    object_free(val);
    
    val = object_list_get(obj, 3);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 3, NULL);
    object_free(val);
    
    val = object_int(4);
    object_list_insert_at(obj, 0, val);
    object_free(val);
    fail_unless(object_list_length(obj) == 5, NULL);
    
    val = object_list_get(obj, 0);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 4, NULL);
    object_free(val);
    
    val = object_list_get(obj, 4);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 3, NULL);
    object_free(val);
    
    object_list_remove(obj, 0);
    
    val = object_list_get(obj, 0);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 0, NULL);
    object_free(val);
    
    object_list_remove(obj, 0);
    
    val = object_list_get(obj, 0);
    fail_unless(object_type(val) == OBJECT_INT, NULL);
    fail_unless(object_int_get(val) == 1, NULL);
    object_free(val);
    
    object_free(obj);
} END_TEST

START_TEST (test_2) {
    object *obj = object_list();
    
    fail_unless(object_list_length(obj) == 0, NULL);
    
    object *val;
    val = object_int(1);
    object_list_insert_at(obj, 0, val);
    object_free(val);
    
    fail_unless(object_list_length(obj) == 1, NULL);
    
    val = object_int(3);
    object_list_insert_at(obj, 1, val);
    object_free(val);
    
    fail_unless(object_list_length(obj) == 2, NULL);
    
    val = object_int(2);
    object_list_insert_at(obj, 1, val);
    object_free(val);
    
    fail_unless(object_list_length(obj) == 3, NULL);
    
    object *next = object_copy(obj);
    
    fail_unless(object_list_length(next) == 3, NULL);
    
    object_list_remove(obj, 0);
    fail_unless(object_list_length(obj) == 2, NULL);
    
    object_list_remove(obj, 0);
    fail_unless(object_list_length(obj) == 1, NULL);
    
    object_list_remove(obj, 0);
    fail_unless(object_list_length(obj) == 0, NULL);
    object_free(obj);
    
    obj = next;
    next = object_copy(next);
    
    object_list_remove(obj, 1);
    fail_unless(object_list_length(obj) == 2, NULL);
    object_free(obj);
    
    obj = next;
    
    object_list_remove(obj, 2);
    fail_unless(object_list_length(obj) == 2, NULL);
    object_free(obj);
    
    object_free(obj);
} END_TEST

TCase *list_test_case() {
    TCase *tc = tcase_create("list");
    tcase_add_test(tc, test_1);
    tcase_add_test(tc, test_2);
    return tc;
}
