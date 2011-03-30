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

SET(EXTRA_LIBRARIES)

OPTION(USE_ICU "Use the ICU Library for Unicode" OFF)

IF(USE_ICU)
	ADD_DEFINITIONS(-DBUTTERFLY_USE_ICU)
	SET(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} icui18n)
	INCLUDE_DIRECTORIES("/usr/include/unicode")
ELSE(USE_ICU)
	ADD_DEFINITIONS(-DBUTTERFLY_USE_ASCII)
ENDIF(USE_ICU)

SET(ButterflySources list map object)
ADD_LIBRARY(butterfly STATIC ${ButterflySources})
ADD_LIBRARY(butterfly_dynamic SHARED ${ButterflySources})
TARGET_LINK_LIBRARIES(butterfly m ${EXTRA_LIBRARIES})
