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
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-compare-cert-" CHECK_TESTCASES_ savelist TCK_COMPARE_INPUT_FILES ${INPUTS})

list(GET TCK_COMPARE_INPUT_FILES 0 edge_case_urgent_cert)
list(GET TCK_COMPARE_INPUT_FILES 1 edge_case_urgent_non_urgent_cert)

## urgent
create_testcase(edge-case-urgent-self-cert ${edge_case_urgent_cert} ${edge_case_urgent_cert} strong-timed-bisim)
create_testcase(edge-case-urgent-non-urgent-self-cert ${edge_case_urgent_non_urgent_cert} ${edge_case_urgent_non_urgent_cert} strong-timed-bisim)
create_testcase(edge-case-urgent-vs-non-urgent-cert ${edge_case_urgent_cert} ${edge_case_urgent_non_urgent_cert} strong-timed-bisim)

tck_add_savelist(save-compare-certificate ${savelist})
