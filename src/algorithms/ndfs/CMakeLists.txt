# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(NDFS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/graph.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/zg-ndfs.cc
    ${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/ndfs/graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/ndfs/stats.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/ndfs/zg-ndfs.hh
    PARENT_SCOPE)
