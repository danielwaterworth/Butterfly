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

#include <math.h>
#include <stdlib.h>

#include "json_serialize_test.h"
#include "object.h"

START_TEST (test_null) {
    object *obj = object_none();
    U_STRING(none_str, "null", 4);
    fail_unless(u_strcmp(none_str, object_to_json(obj, false)) == 0, NULL);
} END_TEST

TCase *json_serialize_test_case() {
    TCase *tc = tcase_create("json_serialization");
    tcase_add_test(tc, test_null);
    return tc;
}
