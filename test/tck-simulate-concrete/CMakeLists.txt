# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# writing special register testcases function for tck-simulate -c

function(create_testcase TEST_NAME model input)
    tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

    set_tests_properties(${TEST_NAME}
                         PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_SIMULATE")

    tck_add_test_envvar(testenv TCK_SIMULATE "${TCK_SIMULATE}")
    tck_add_test_envvar(testenv TEST "${TCK_SIMULATE_SH}")
    tck_add_test_envvar(testenv TEST_ARGS "-c ${model} < ${input}")
    tck_set_test_env(${TEST_NAME} testenv)
    unset(testenv)
endfunction(create_testcase)

option(TCK_ENABLE_CONCRETE_SIMULATE_TESTS "enable tests related to tck-simulate with concrete values" ON)

if(NOT TCK_ENABLE_CONCRETE_SIMULATE_TESTS)
    message(STATUS "concrete tck-simulate tests are disabled.")
    return()
endif()

# Elements of INPUTS are colon-separated lists. The first element of each
# list is a test-case script located in ${EXAMPLES_DIR}; the tail of the list
# contains arguments passed to the generator script.
# only append to the tail of this list!
set(INPUTS
    ad94.sh:
    train_gate.sh:3
    dining-philosophers.sh:3:3:10:0
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-simulate-concrete-" CHECK_TESTCASES_ savelist TCK_SIMULATE_CONCRETE_INPUT_FILES ${INPUTS})

list(GET TCK_SIMULATE_CONCRETE_INPUT_FILES 0 ad94)
list(GET TCK_SIMULATE_CONCRETE_INPUT_FILES 1 train-gate-3)
list(GET TCK_SIMULATE_CONCRETE_INPUT_FILES 2 dining-philosophers-3-3-10-0)

create_testcase(tck-simulate-concrete-ad94-first ${ad94} ${CMAKE_SOURCE_DIR}/test/tck-simulate-concrete/test_input_ad_94.in)
create_testcase(tck-simulate-concrete-ad94-second ${ad94} ${CMAKE_SOURCE_DIR}/test/tck-simulate-concrete/test_input_ad_94_second.in)

create_testcase(tck-simulate-concrete-train_gate_3-comm ${train-gate-3} ${CMAKE_SOURCE_DIR}/test/tck-simulate-concrete/test_input_train_gate_3.in)

create_testcase(tck-simulate-concrete-dining-philosophers_3_3_10_0-eat-acq ${dining-philosophers-3-3-10-0} ${CMAKE_SOURCE_DIR}/test/tck-simulate-concrete/test_input_dining_philosophers_3_3_10_0.in)
