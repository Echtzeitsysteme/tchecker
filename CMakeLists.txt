# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

cmake_minimum_required(VERSION 3.13)

set(LOCAL_CONFIG "${CMAKE_SOURCE_DIR}/localconfig-${CMAKE_BUILD_TYPE}.cmake")

if(EXISTS ${LOCAL_CONFIG})
    cmake_policy(SET CMP0077 NEW)
    message(STATUS "Loading ${LOCAL_CONFIG} file.")
    include(${LOCAL_CONFIG})
else()
    message(STATUS "no ${LOCAL_CONFIG} file.")
endif()

project(tchecker LANGUAGES CXX)

if (USE_CPPCHECK)
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
    if(NOT ${CMAKE_CXX_CPPCHECK} STREQUAL "NOTFOUND")
        message(STATUS "using CPPCHECK")
        set(CPPCHECK_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/cppcheck-build-dir")
        file(MAKE_DIRECTORY ${CPPCHECK_BUILD_DIR})
        list(
            APPEND CMAKE_CXX_CPPCHECK 
                "--enable=warning"
                "--inconclusive"
                "--force" 
                "--inline-suppr"
                "--cppcheck-build-dir=${CPPCHECK_BUILD_DIR}"
                "--error-exitcode=1"
                "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
        )
    else()
      message(FATAL_ERROR "wanted to use CPPCHECK, but not installed")
    endif()
else()
    message(STATUS "CPP_CHECK is not actived. If you want to activate it, use -DUSE_CPPCHECK=1")
endif()

set(TCK_CMAKE_SCRIPT_DIR "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${TCK_CMAKE_SCRIPT_DIR}")

include(CheckCompilerFlags)
include(TcheckerUtils)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   8   CACHE STRING "Project minor version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR)

add_subdirectory(include)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
