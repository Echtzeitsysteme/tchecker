# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

add_subdirectory(witness)

set(STRONG_TIMED_BISIM_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/algorithm_return_value.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/non_bisim_cache.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/virtual_clock_algorithm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/system.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/contradiction_searcher.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/visited_map.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/vcg-timed-bisim.cc
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/stats.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/algorithm_return_value.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/non_bisim_cache.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/virtual_clock_algorithm.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/system.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/contradiction_searcher.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/visited_map.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/vcg-timed-bisim.hh
    ${STRONG_TIMED_BISIM_WITNESS_SRC}
    PARENT_SCOPE)