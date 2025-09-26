# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# writing special register testcases function for tck-compare certificates
# this is neccessary since the original register testcases function accepts a single
# input model only

function(create_testcase TEST_NAME first_input second_input relationship)
    tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

    set_tests_properties(${TEST_NAME}
                         PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_COMPARE")

    tck_add_test_envvar(testenv TCK_COMPARE "${TCK_COMPARE}")
    tck_add_test_envvar(testenv TEST "${TCK_COMPARE_SH}")
    tck_add_test_envvar(testenv TEST_ARGS "-W -r ${relationship} ${first_input} ${second_input}")
    tck_add_test_envvar(testenv DOT_MAX_SIZE "${DOT_MAX_SIZE}")
    tck_set_test_env(${TEST_NAME} testenv)
    unset(testenv)
endfunction(create_testcase)

option(TCK_ENABLE_COMPARE_CERT_TESTS "enable tests related to tck_compare_cert" ON)

if(NOT TCK_ENABLE_COMPARE_CERT_TESTS)
    message(STATUS "tck_compare_cert tests are disabled.")
    return()
endif()


# Max number of lines for DOT results
set(DOT_MAX_SIZE 20000)

# Elements of INPUTS are colon-separated lists. The first element of each
# list is a test-case script located in ${EXAMPLES_DIR}; the tail of the list
# contains arguments passed to the generator script.
# only append to the tail of this list!
set(INPUTS
    strong_timed_bisim_system_tests_edge_case_urgent_cert.sh:
    strong_timed_bisim_system_tests_edge_case_urgent_non_urgent_cert.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-bisim.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-non-bisim-changed-guard.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-non-bisim-changed-invariant.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-non-bisim-removed-reset.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-bisim.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-non-bisim-changed-guard.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-non-bisim-changed-invariant.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-non-bisim-removed-reset.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-bisim.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-non-bisim-changed-guard.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-non-bisim-changed-invariant.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-non-bisim-removed-reset.sh:
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-compare-cert-" CHECK_TESTCASES_ savelist TCK_COMPARE_INPUT_FILES ${INPUTS})

list(GET TCK_COMPARE_INPUT_FILES 0 edge_case_urgent_cert)
list(GET TCK_COMPARE_INPUT_FILES 1 edge_case_urgent_non_urgent_cert)
list(GET TCK_COMPARE_INPUT_FILES 2 benchmark_determ_av-protocol_cert)
list(GET TCK_COMPARE_INPUT_FILES 3 benchmark_determ_av-protocol-mutant-bisim_cert)
list(GET TCK_COMPARE_INPUT_FILES 4 benchmark_determ_av-protocol-mutant-non-bisim-changed-guard_cert)
list(GET TCK_COMPARE_INPUT_FILES 5 benchmark_determ_av-protocol-mutant-non-bisim-changed-inv_cert)
list(GET TCK_COMPARE_INPUT_FILES 6 benchmark_determ_av-protocol-mutant-non-bisim-rm-reset_cert)
list(GET TCK_COMPARE_INPUT_FILES 7 benchmark_determ_collision-avoidance_cert)
list(GET TCK_COMPARE_INPUT_FILES 8 benchmark_determ_collision-avoidance-mutant-bisim_cert)
list(GET TCK_COMPARE_INPUT_FILES 9 benchmark_determ_collision-avoidance-mutant-non-bisim-changed-guard_cert)
list(GET TCK_COMPARE_INPUT_FILES 10 benchmark_determ_collision-avoidance-mutant-non-bisim-changed-inv_cert)
list(GET TCK_COMPARE_INPUT_FILES 11 benchmark_determ_collision-avoidance-mutant-non-bisim-rm-reset_cert)
list(GET TCK_COMPARE_INPUT_FILES 12 benchmark_determ_ieee-rcp_cert)
list(GET TCK_COMPARE_INPUT_FILES 13 benchmark_determ_ieee-rcp-mutant-bisim_cert)
list(GET TCK_COMPARE_INPUT_FILES 14 benchmark_determ_ieee-rcp-mutant-non-bisim-changed-guard_cert)
list(GET TCK_COMPARE_INPUT_FILES 15 benchmark_determ_ieee-rcp-mutant-non-bisim-changed-inv_cert)
list(GET TCK_COMPARE_INPUT_FILES 16 benchmark_determ_ieee-rcp-mutant-non-bisim-rm-reset_cert)

## urgent
create_testcase(edge-case-urgent-self-cert ${edge_case_urgent_cert} ${edge_case_urgent_cert} strong-timed-bisim)
create_testcase(edge-case-urgent-non-urgent-self-cert ${edge_case_urgent_non_urgent_cert} ${edge_case_urgent_non_urgent_cert} strong-timed-bisim)
create_testcase(edge-case-urgent-vs-non-urgent-cert ${edge_case_urgent_cert} ${edge_case_urgent_non_urgent_cert} strong-timed-bisim)

# av-protocol (can only be used for int size > 16, since the largest occuring number, 50000, is too large)
if(NOT USEINT16)
    create_testcase(av-protocol-self-cert ${benchmark_determ_av-protocol_cert} ${benchmark_determ_av-protocol_cert} strong-timed-bisim)
    create_testcase(av-protocol-av-protocol-bisim-cert ${benchmark_determ_av-protocol_cert} ${benchmark_determ_av-protocol-mutant-bisim_cert} strong-timed-bisim)
    create_testcase(av-protocol-av-protocol-non-bisim-changed-guard-cert ${benchmark_determ_av-protocol_cert} ${benchmark_determ_av-protocol-mutant-non-bisim-changed-guard_cert} strong-timed-bisim)
    create_testcase(av-protocol-av-protocol-non-bisim-changed-invariant-cert ${benchmark_determ_av-protocol_cert} ${benchmark_determ_av-protocol-mutant-non-bisim-changed-inv_cert} strong-timed-bisim)
    create_testcase(av-protocol-av-protocol-non-bisim-removed-reset-cert ${benchmark_determ_av-protocol_cert} ${benchmark_determ_av-protocol-mutant-non-bisim-rm-reset_cert} strong-timed-bisim)
endif()

# CA
create_testcase(ca-self-cert ${benchmark_determ_collision-avoidance_cert} ${benchmark_determ_collision-avoidance_cert} strong-timed-bisim)
create_testcase(ca-ca-bisim-cert ${benchmark_determ_collision-avoidance_cert} ${benchmark_determ_collision-avoidance-mutant-bisim_cert} strong-timed-bisim)
create_testcase(ca-ca-non-bisim-changed-guard-cert ${benchmark_determ_collision-avoidance_cert} ${benchmark_determ_collision-avoidance-mutant-non-bisim-changed-guard_cert} strong-timed-bisim)
create_testcase(ca-ca-non-bisim-changed-invariant-cert ${benchmark_determ_collision-avoidance_cert} ${benchmark_determ_collision-avoidance-mutant-non-bisim-changed-inv_cert} strong-timed-bisim)
create_testcase(ca-ca-non-bisim-removed-reset-cert ${benchmark_determ_collision-avoidance_cert} ${benchmark_determ_collision-avoidance-mutant-non-bisim-rm-reset_cert} strong-timed-bisim)

# ieee-rcp
create_testcase(ieee-rcp-self-cert ${benchmark_determ_ieee-rcp_cert} ${benchmark_determ_ieee-rcp_cert} strong-timed-bisim)
create_testcase(ieee-rcp-ieee-rcp-bisim-cert ${benchmark_determ_ieee-rcp_cert} ${benchmark_determ_ieee-rcp-mutant-bisim_cert} strong-timed-bisim)
create_testcase(ieee-rcp-ieee-rcp-non-bisim-changed-guard-cert ${benchmark_determ_ieee-rcp_cert} ${benchmark_determ_ieee-rcp-mutant-non-bisim-changed-guard_cert} strong-timed-bisim)
create_testcase(ieee-rcp-ieee-rcp-non-bisim-changed-invariant-cert ${benchmark_determ_ieee-rcp_cert} ${benchmark_determ_ieee-rcp-mutant-non-bisim-changed-inv_cert} strong-timed-bisim)
create_testcase(ieee-rcp-ieee-rcp-non-bisim-removed-reset-cert ${benchmark_determ_ieee-rcp_cert} ${benchmark_determ_ieee-rcp-mutant-non-bisim-rm-reset_cert} strong-timed-bisim)

tck_add_savelist(save-compare-certificate ${savelist})
