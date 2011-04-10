#    Copyright (C) 2011 Butterfly authors,
#    
#    This file is part of Butterfly.
#    
#    Butterfly is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Butterfly is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(EXTRA_LIBRARIES m) #libm is required for pow and other math functions

OPTION(USE_ICU "Use the ICU Library for Unicode" ON)
OPTION(BUILD_STATIC_LIB "Build the static Butterfly lib" ON)
OPTION(BUILD_SHARED_LIB "Build the shared Butterfly lib" ON)
OPTION(BUILD_UNITTESTS "Build the Unittests (recommented)" ON)

#if ICU is used, use icu library and define BUTTERFLY_USE_ICU
IF(USE_ICU)
	ADD_DEFINITIONS(-DBUTTERFLY_USE_ICU)
	SET(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} icui18n)
	INCLUDE_DIRECTORIES("/usr/include/unicode")
ELSE(USE_ICU)
	#else define BUTTERFLY_USE_ASCII
	ADD_DEFINITIONS(-DBUTTERFLY_USE_ASCII)
ENDIF(USE_ICU)

#the sources for the library
SET(ButterflySources list map object string_type)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

if(BUILD_UNITTESTS)
	#the sources for the unittests
	SET(UnittestSources tests/iterator_test tests/json_deserialize_test tests/json_serialize_test tests/list_test tests/map_test tests/primitive_test tests/string_test)	
	INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/tests")
	SET(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} check)
endif(BUILD_UNITTESTS)

#shared lib and unittests for shared lib
IF(BUILD_SHARED_LIB)
	ADD_LIBRARY(butterfly_dynamic SHARED ${ButterflySources})
	if(BUILD_UNITTESTS)
		ADD_EXECUTABLE(test_shared tests.c ${UnittestSources})
		TARGET_LINK_LIBRARIES(test_shared butterfly_dynamic ${EXTRA_LIBRARIES})
	endif(BUILD_UNITTESTS)
ENDIF(BUILD_SHARED_LIB)

#static lib and unittests for static lib
IF(BUILD_STATIC_LIB)
	ADD_LIBRARY(butterfly STATIC ${ButterflySources})
	if(BUILD_UNITTESTS)
		ADD_EXECUTABLE(test tests.c ${UnittestSources})
		TARGET_LINK_LIBRARIES(test butterfly ${EXTRA_LIBRARIES})
	endif(BUILD_UNITTESTS)
ENDIF(BUILD_STATIC_LIB)
