# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

# SPDX-License-Identifier: MPL-2.0

cmake_minimum_required(VERSION 3.21)
project(CAlloc C)

# this file does kinda suck but it also does work and i can't be bothered to make it decent

option(C_ALLOC_USE_ASAN "Links c-alloc and any of its tests with the ASAN library.")
option(C_ALLOC_DEBUG "Builds a debug version of c-alloc and adds extra GCC/Clang-specific compiler flags.")
option(C_ALLOC_CPPCHECK "Uses the cppcheck tool to statically analyze source code when building.")
option(C_ALLOC_CLANG_TIDY "Uses the clang-tidy tool to statically analyze source code when building.")
option(C_ALLOC_TESTS "Builds c-alloc's unit tests.")

if(C_ALLOC_TESTS)
	include(CTest)
endif()


if(C_ALLOC_USE_ASAN)
	message(NOTICE "Option `C_ALLOC_USE_ASAN` specified. Linking ASAN and setting `C_ALLOC_DEBUG`.")
	set(CMAKE_CXX_FLAGS "-fsanitize=address -fsanitize-recover=address -fsanitize=undefined -fsanitize=leak")
	set(C_ALLOC_DEBUG ON)
endif()

if (C_ALLOC_DBGFLAGS)
	message(NOTICE "Option `C_ALLOC_DBGFLAGS` specified. Adding extra compile flags.")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
endif()

if (C_ALLOC_CPPCHECK)
	find_program(CPPCHECK cppcheck)
	if (CPPCHECK)
		message(NOTICE "Found cppcheck!")
		list(APPEND CMAKE_CXX_CPPCHECK
			"--enable=all"
			"--inconclusive"
			"--force"
			"--std=c11"
		)
	else()
		message(WARNING "Failed to find cppcheck. Continuing...")
	endif()
endif()

if (C_ALLOC_CLANG_TIDY)
	find_program(CLANG_TIDY clang-tidy)
	if (CLANG_TIDY)
		message(NOTICE "Found clang-tidy!")
		list(APPEND CMAKE_C_CLANG_TIDY
			"--use-color"
		)
		set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY})
	else()
		message(NOTICE "clang-tidy not found and not used.")
	endif()
endif()



function(make_test testfilename) # can take libs to link as an optional arg
if (C_ALLOC_TESTS)
	get_filename_component(EXENAME ${testfilename} NAME_WLE)
	add_executable(${EXENAME} ${testfilename})
	target_link_libraries(${EXENAME} ${ARGN})
	add_test(
		NAME ${EXENAME}
		COMMAND $<TARGET_FILE:${EXENAME}>
	)
endif()
endfunction()

add_subdirectory(src)
