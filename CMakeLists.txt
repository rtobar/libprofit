# CMakeLists.txt
#
# ICRAR - International Centre for Radio Astronomy Research
# (c) UWA - The University of Western Australia, 2016
# Copyright by UWA (in the framework of the ICRAR)
# All rights reserved
#
# Contributed by Rodrigo Tobar
#
# This file is part of libprofit.
#
# libprofit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libprofit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libprofit.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.1)

# We have the version here
file(STRINGS ${CMAKE_SOURCE_DIR}/VERSION PROFIT_VERSION)
string(REPLACE "." ";" VERSION_LIST ${PROFIT_VERSION})
list(GET VERSION_LIST 0 PROFIT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROFIT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROFIT_VERSION_PATCH)

# We have only C++ sources, but until cmake 3.4 the check_symbol_exists
# macro didn't work properly if only defining C++ as the project language
if( ${CMAKE_MAJOR_VERSION} EQUAL 3 AND ${CMAKE_MINOR_VERSION} LESS 4 )
	set(PROFIT_LANGS C CXX)
else()
	set(PROFIT_LANGS CXX)
endif()

# Our project...
project(libprofit VERSION ${PROFIT_VERSION} LANGUAGES ${PROFIT_LANGS})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# Options users can give on the command line
option(LIBPROFIT_USE_R     "Try to compile libprofit first against R/Rmath, and then with GSL" OFF)
option(LIBPROFIT_TEST      "Include test compilation in the build" OFF)
option(LIBPROFIT_DEBUG     "Include some debugging code in libprofit" OFF)
option(LIBPROFIT_NO_OPENCL "Don't attempt to include OpenCL support in libprofit" OFF)
option(LIBPROFIT_NO_OPENMP "Don't attempt to include OpenMP support in libprofit" OFF)
option(LIBPROFIT_NO_FFTW   "Don't attempt to include FFTW support in libprofit" OFF)

#
# Macros to check for the presence of:
#  * GSL
#  * R/Rmath
#  * OpenCL
#  * OpenMP
#  * FFTW
#
# From those, only GSL or R/Rmath are required (one of the two), the rest is
# optional.
#
macro(find_gsl)

	# Try with FindGSL.cmake first, if found
	# Otherwise try with pkg-config
	find_package(GSL)
	if( NOT GSL_FOUND )
		find_package(PkgConfig)
		if( PKG_CONFIG_FOUND )
			pkg_check_modules(GSL gsl)
		endif()
	endif()

	if( GSL_FOUND )
		set(PROFIT_USES_GSL ON)
		include_directories(${GSL_INCLUDE_DIRS})
		set(profit_LIBS ${GSL_LIBRARIES})
	endif()
endmacro()

macro(find_r_and_rmath)
	find_package(PkgConfig)
	if( PKG_CONFIG_FOUND )
		pkg_check_modules(R libR)
		pkg_check_modules(RMATH libRmath)
		if( R_FOUND AND RMATH_FOUND )
			set(PROFIT_USES_R ON)
			add_definitions(-DMATHLIB_STANDALONE)
			include_directories(${R_INCLUDE_DIRS} ${RMATH_INCLUDE_DIRS})
			set(profit_LIBS ${R_LIBRARIES} ${RMATH_LIBRARIES})
		endif( R_FOUND AND RMATH_FOUND )
	endif( PKG_CONFIG_FOUND )
endmacro()

macro(find_opencl)
	find_package(OpenCL)
	if( OpenCL_FOUND )
		include_directories(${OpenCL_INCLUDE_DIRS})
		set(PROFIT_OPENCL ON)
		set(PROFIT_OPENCL_MAJOR ${OpenCL_VERSION_MAJOR})
		set(PROFIT_OPENCL_MINOR ${OpenCL_VERSION_MINOR})
		set(profit_LIBS ${profit_LIBS} ${OpenCL_LIBRARIES})
	endif()
endmacro()

macro(find_openmp)
	find_package(OpenMP)
	if( OPENMP_FOUND )
		set(PROFIT_OPENMP ON)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	endif()
endmacro()

macro(find_fftw)
	find_package(FFTW)
	if( FFTW_FOUND )
		set(PROFIT_FFTW ON)
		if( FFTW_OPENMP_FOUND )
			set(PROFIT_FFTW_OPENMP ON)
		endif()
		include_directories(${FFTW_INCLUDE_DIR})
		set(profit_LIBS ${profit_LIBS} ${FFTW_LIBRARIES})
	endif()
endmacro(find_fftw)

#
# Depending on the user's preference we check GSL or R/Rmath first
# We default to use GSL
#
if( LIBPROFIT_USE_R )
	find_r_and_rmath()
	if( NOT R_FOUND OR NOT RMATH_FOUND)
		find_gsl()
	endif()
else()
	find_gsl()
	if( NOT GSL_FOUND )
		find_r_and_rmath()
	endif()
endif()

#
# None found, what an ill luck
if( NOT GSL_FOUND )
	if( NOT R_FOUND OR NOT RMATH_FOUND )
		message(FATAL_ERROR "Neither GSL nor R/Rmath were found, cannot continue")
	endif()
endif()

#
# Check if there's OpenCL (users might opt out)
#
if( NOT LIBPROFIT_NO_OPENCL )
	find_opencl()
endif()

#
# Check if there's OpenMP support (users might opt out)
#
if( NOT LIBPROFIT_NO_OPENMP )
	find_openmp()
endif()

#
# Check if there's FFTW support (users might opt out)
#
if( NOT LIBPROFIT_NO_FFTW )
	find_fftw()
endif()

#
# Include debugging stuff? (off by default)
#
if( LIBPROFIT_DEBUG )
	set(PROFIT_DEBUG ON)
endif()

#
# Create the final configuration file that records
# the compile-time options used for this libprofit build
#
configure_file(profit/config.h.in profit/config.h @ONLY)

#
# Common definitions for the shared lib and the binary
#
add_definitions(-DPROFIT_BUILD)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR})

#
# The shared library sources
#
set(PROFIT_SRC
   src/brokenexponential.cpp
   src/convolve.cpp
   src/coresersic.cpp
   src/exceptions.cpp
   src/ferrer.cpp
   src/fft.cpp
   src/image.cpp
   src/king.cpp
   src/model.cpp
   src/moffat.cpp
   src/opencl.cpp
   src/profile.cpp
   src/psf.cpp
   src/radial.cpp
   src/sersic.cpp
   src/sky.cpp
   src/utils.cpp
)
add_library(profit SHARED ${PROFIT_SRC})
target_link_libraries(profit ${profit_LIBS})

#
# The executable
#
add_executable(profit-cli src/profit-cli.cpp)
target_link_libraries(profit-cli profit)

#
# Installing lib + binary + headers
#
install(TARGETS profit profit-cli
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
install(FILES
       ${CMAKE_CURRENT_BINARY_DIR}/profit/config.h
        profit/common.h
        profit/convolve.h
        profit/exceptions.h
        profit/fft.h
        profit/image.h
        profit/model.h
        profit/opencl.h
        profit/profile.h
        profit/profit.h
        DESTINATION include/profit)
install(FILES
        profit/cl/cl2.hpp
        DESTINATION include/profit/cl)

#
# Unit tests
#
if( LIBPROFIT_TEST )
	find_package(CxxTest)
	if( CXXTEST_FOUND )
		enable_testing()
		add_subdirectory(tests)
	endif()
endif()
