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

#include "check.h"

#include "list_test.h"
#include "map_test.h"
#include "primitive_test.h"
#include "json_deserialize_test.h"
#include "json_serialize_test.h"
#include "string_test.h"
#include "iterator_test.h"

int main() {
    int number_failed;
    Suite *s = suite_create("Main");
    
    suite_add_tcase(s, list_test_case());
    suite_add_tcase(s, map_test_case());
    suite_add_tcase(s, primitive_test_case());
    suite_add_tcase(s, json_deserialize_test_case());
    suite_add_tcase(s, json_serialize_test_case());
    suite_add_tcase(s, string_test_case());
    suite_add_tcase(s, iterator_test_case());
    
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}
