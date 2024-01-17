# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# writing special register testcases function for tck-compare
# this is neccessary since the original register testcases function accepts a single
# input model only

function(create_testcase TEST_NAME first_input second_input relationship)
    tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

    set_tests_properties(${TEST_NAME}
                         PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_COMPARE")

    tck_add_test_envvar(testenv TCK_COMPARE "${TCK_COMPARE}")
    tck_add_test_envvar(testenv TEST "${TCK_COMPARE_SH}")
    tck_add_test_envvar(testenv TEST_ARGS "-r ${relationship} ${first_input} ${second_input}")
    tck_add_test_envvar(testenv DOT_MAX_SIZE "${DOT_MAX_SIZE}")
    tck_set_test_env(${TEST_NAME} testenv)
    unset(testenv)
endfunction(create_testcase)


option(TCK_ENABLE_COMPARE_TESTS "enable tests related to tck_compare" ON)

if(NOT TCK_ENABLE_COMPARE_TESTS)
    message(STATUS "tck_compare tests are disabled.")
    return()
endif()


# Max number of lines for DOT results
set(DOT_MAX_SIZE 20000)

# Elements of INPUTS are colon-separated lists. The first element of each
# list is a test-case script located in ${EXAMPLES_DIR}; the tail of the list
# contains arguments passed to the generator script.
# only append to the tail of this list!
set(INPUTS
    missing_initial.sh:
    ad94.sh:
    corsso.sh:2:2:10:1:2
    critical-region-async.sh:2:10
    csmacd.sh:3
    dining-philosophers.sh:3:3:10:0
    fischer-async.sh:3:10
    fischer-async-concurrent.sh:3:10
    parallel-c.sh:3
    train_gate.sh:2
    train_gate.sh:3
    )

if(NOT USEINT16)
    list(APPEND INPUTS
         ad94_mid.sh:)
endif()

if(USEINT64)
    list(APPEND INPUTS
         ad94_Long.sh:)
endif()

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-compare-" CHECK_TESTCASES_ savelist TCK_COMPARE_INPUT_FILES ${INPUTS})

# TODO: Insert real Testcases here. You can find the name of all the available files in TCK_COMPARE_INPUT_FILES or add some to INPUTS

list(GET TCK_COMPARE_INPUT_FILES 0 no_initial)
list(GET TCK_COMPARE_INPUT_FILES 1 ad)

create_testcase(no_initial_first ${no_initial} ${ad} strong-timed-bisim)
create_testcase(no_initial_second ${ad} ${no_initial} strong-timed-bisim)




tck_add_savelist(save-compare ${savelist})
