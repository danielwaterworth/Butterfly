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

#include "map_test.h"
#include "object.h"

START_TEST (test_1) {
    object *obj = object_map();
    
    STR_INIT(hello, "hello", 5);
    STR_INIT(world, "world", 5);
    
    object *key = object_str(hello);
    object *val = object_str(world);
    object_map_set(obj, key, val);
    
    object *out = object_map_get(obj, key);
    fail_unless(object_eq(val, out), NULL);
    object_free(out);
    object_free(key);
    object_free(val);
    
    object_free(obj);
} END_TEST

TCase *map_test_case() {
    TCase *tc = tcase_create("map");
    tcase_add_test(tc, test_1);
    return tc;
}
