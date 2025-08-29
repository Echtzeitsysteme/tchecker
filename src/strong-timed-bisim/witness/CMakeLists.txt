# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(STRONG_TIMED_BISIM_WITNESS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/witness_edge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/witness_graph.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/witness_node.cc
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/witness/witness_edge.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/witness/witness_graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/witness/witness_node.hh
    PARENT_SCOPE)