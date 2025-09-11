add_subdirectory(contradiction)
add_subdirectory(witness)

set(STRONG_TIMED_BISIM_CERT_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/certificate_node.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/clock_names.cc
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/certificate_node.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/certificate_edge.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/certificate_graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/strong-timed-bisim/certificate/clock_names.hh
    ${STRONG_TIMED_BISIM_CONTRADICTION_SRC}
    ${STRONG_TIMED_BISIM_WITNESS_SRC}
    PARENT_SCOPE)