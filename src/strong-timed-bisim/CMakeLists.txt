# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(STRONG_TIMED_BISIM_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithm_return_value.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/non_bisim_cache.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/virtual_clock_algorithm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/system.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/contradiction_searcher.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/visited_map.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vcg-timed-bisim.cc
    PARENT_SCOPE)
