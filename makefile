#   Copyright (C) 2011 Daniel Waterworth
#   
#   This file is part of Butterfly.
#   
#   Butterfly is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   Butterfly is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

srcs=object.c list.c map.c
objs=object.o list.o map.o

test_srcs=tests.c tests/list_test.c tests/primitive_test.c tests/map_test.c \
          tests/json_deserialize_test.c tests/json_serialize_test.c
all:
	gcc ${srcs} -c -I/usr/include/unicode

check:
	gcc ${srcs} ${test_srcs} -I/usr/include/unicode -I./ -I./tests/ -o test \
	-licui18n -lcheck
	./test
