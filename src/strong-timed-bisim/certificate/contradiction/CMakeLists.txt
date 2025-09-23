# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(STRONG_TIMED_BISIM_CONTRADICTION_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/cont_dag.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/cont_node.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/cont_edge.cc
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/contradiction/cont_edge.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/contradiction/cont_dag.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/contradiction/cont_node.hh
    PARENT_SCOPE)