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

#include "string_test.h"
#include "string_type.h"

START_TEST (init_test) {
    STR_INIT(str, "hello world", 11);
} END_TEST

START_TEST (convert_test) {
    char_t str[12];
    str_convert("hello world", str, 12);
} END_TEST

TCase *string_test_case() {
    TCase *tc = tcase_create("string");
    tcase_add_test(tc, init_test);
    tcase_add_test(tc, convert_test);
    return tc;
}
