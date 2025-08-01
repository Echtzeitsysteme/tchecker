# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

include(CheckCXXCompilerFlag)

# Options to support 16/32/64 bit dbm's
# 32 is default
option(USEINT16 "Use 16bit int" OFF)
option(USEINT64 "Use 64bit int" OFF)

set(INTEGER_T_SIZE 32)
if (USEINT16)
    set(INTEGER_T_SIZE 16)
elseif(USEINT64)
    set(INTEGER_T_SIZE 64)
else()
endif()

message(STATUS "Setting sizeof(integer_t) to ${INTEGER_T_SIZE}")

option(SUBSETS_WITH_NEG_AND "Use subsets with negation." OFF)
option(SUBSETS_WITH_INTERSECTIONS "Use subsets with intersections." OFF)
option(SUBSETS_WITH_COMPRESS "Use subsets with compress." OFF)
option(SUBSETS_WITHOUT_COMPRESS "Use subsets without compress." OFF)
option(WITHOUT_SUBSETS "Use equivalence." OFF)

set(SUBSET_COUNTER 0)

message(STATUS "Setting subset behaviour for timed bisimulation check: ")

if(SUBSETS_WITH_NEG_AND)
    message(STATUS "SUBSETS_WITH_NEG_AND is set. We use subsets with negation.")
    MATH(EXPR SUBSET_COUNTER "${SUBSET_COUNTER}+1")
endif()

if(SUBSETS_WITH_INTERSECTIONS)
    message(STATUS "SUBSETS_WITH_INTERSECTIONS is set. We use subsets with intersections.")
    MATH(EXPR SUBSET_COUNTER "${SUBSET_COUNTER}+1")
endif()

if(SUBSETS_WITH_COMPRESS)
    message(STATUS "SUBSETS_WITH_COMPRESS is set. We use subsets with compress.")
    MATH(EXPR SUBSET_COUNTER "${SUBSET_COUNTER}+1")
endif()

if(SUBSETS_WITHOUT_COMPRESS)
    message(STATUS "SUBSETS_WITHOUT_COMPRESS is set. We use subsets without compress.")
    MATH(EXPR SUBSET_COUNTER "${SUBSET_COUNTER}+1")
endif()

if(WITHOUT_SUBSETS)
    message(STATUS "WITHOUT_SUBSETS is set. We check for equivalence.")
    MATH(EXPR SUBSET_COUNTER "${SUBSET_COUNTER}+1")
endif()

if(SUBSET_COUNTER LESS 1)
    set(WITHOUT_SUBSETS ON)
    message(STATUS "No behaviour defined. Defaulting to check for equivalence.")
endif()

if(SUBSET_COUNTER GREATER 1)
    message(FATAL_ERROR "Multiple subset behaviour defined! Disable one of the previously mentioned subset policies or try --fresh.")
endif()

#
# Check if "flag" is accepted by the current CXX compiler. If the flag is
# supported its value is assigned to the variable "var"; else "var" is asigned
# an empty string.
#
function(tck_check_cxx_flags flag var)
    check_cxx_compiler_flag("${flag}" ${var}_IS_SUPPORTED)
    if(${var}_IS_SUPPORTED)
        set(${var} ${flag} PARENT_SCOPE)
    else()
        set(${var} "")
    endif()
endfunction()
