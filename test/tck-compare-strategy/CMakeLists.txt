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
    tck_add_test_envvar(testenv TEST_ARGS "-r ${relationship} --allReachableStates ${first_input} ${second_input}")
    tck_add_test_envvar(testenv DOT_MAX_SIZE "${DOT_MAX_SIZE}")
    tck_set_test_env(${TEST_NAME} testenv)
    unset(testenv)
endfunction(create_testcase)


option(TCK_ENABLE_COMPARE_STRATEGY_TESTS "enable tests related to tck_compare" ON)

if(NOT TCK_ENABLE_COMPARE_STRATEGY_TESTS)
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
    strong_timed_bisim_system_tests/edge_cases/contradiction_region_test.sh
    strong_timed_bisim_system_tests/edge_cases/contradiction_region_test_mutant.sh
    strong_timed_bisim_benchmarks/deterministic/av-protocol.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-bisim.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-non-bisim-changed-guard.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-non-bisim-changed-invariant.sh:
    strong_timed_bisim_benchmarks/deterministic/av-protocol-mutants/av-protocol-non-bisim-removed-reset.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-bisim.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-non-bisim-changed-guard.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-non-bisim-changed-invariant.sh:
    strong_timed_bisim_benchmarks/deterministic/collision-avoidance-mutants/collision-avoidance-non-bisim-removed-reset.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-bisim.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-non-bisim-changed-guard.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-non-bisim-changed-invariant.sh:
    strong_timed_bisim_benchmarks/deterministic/ieee-rcp-mutants/ieee-rcp-non-bisim-removed-reset.sh:
    strong_timed_bisim_system_tests/edge_cases/urgent.sh:
    strong_timed_bisim_system_tests/edge_cases/urgent_non_urgent.sh:
    )


file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-compare-strat-" CHECK_TESTCASES_ savelist TCK_COMPARE_INPUT_FILES ${INPUTS})

list(GET TCK_COMPARE_INPUT_FILES 0 tck-compare-strat-contradiction_region)
list(GET TCK_COMPARE_INPUT_FILES 1 tck-compare-strat-contradiction_region_mutant)
list(GET TCK_COMPARE_INPUT_FILES 2 tck-compare-strat-av-protocol)
list(GET TCK_COMPARE_INPUT_FILES 3 tck-compare-strat-av-protocol-bisim)
list(GET TCK_COMPARE_INPUT_FILES 4 tck-compare-strat-av-protocol-non-bisim-changed-guard)
list(GET TCK_COMPARE_INPUT_FILES 5 tck-compare-strat-av-protocol-non-bisim-changed-invariant)
list(GET TCK_COMPARE_INPUT_FILES 6 tck-compare-strat-av-protocol-non-bisim-removed-reset)
list(GET TCK_COMPARE_INPUT_FILES 7 tck-compare-strat-collision-avoidance-bisim)
list(GET TCK_COMPARE_INPUT_FILES 8 tck-compare-strat-collision-avoidance-non-bisim-changed-guard)
list(GET TCK_COMPARE_INPUT_FILES 9 tck-compare-strat-collision-avoidance-non-bisim-changed-invariant)
list(GET TCK_COMPARE_INPUT_FILES 10 tck-compare-strat-collision-avoidance-non-bisim-removed-reset)
list(GET TCK_COMPARE_INPUT_FILES 11 tck-compare-strat-ieee-rcp)
list(GET TCK_COMPARE_INPUT_FILES 12 tck-compare-strat-ieee-rcp-bisim)
list(GET TCK_COMPARE_INPUT_FILES 13 tck-compare-strat-ieee-rcp-non-bisim-changed-guard)
list(GET TCK_COMPARE_INPUT_FILES 14 tck-compare-strat-ieee-rcp-non-bisim-changed-invariant)
list(GET TCK_COMPARE_INPUT_FILES 15 tck-compare-strat-ieee-rcp-non-bisim-removed-reset)
list(GET TCK_COMPARE_INPUT_FILES 16 tck-compare-strat-urgent)
list(GET TCK_COMPARE_INPUT_FILES 17 tck-compare-strat-urgent_non_urgent)

# check whether the no_initial_check works
create_testcase(contradiction_region_with_mutant ${tck-compare-strat-contradiction_region} ${tck-compare-strat-contradiction_region_mutant} strong-timed-bisim)
create_testcase(contradiction_region_self ${tck-compare-strat-contradiction_region} ${tck-compare-strat-contradiction_region} strong-timed-bisim)

# av-protocol benchmark variants
create_testcase(av-protocol-self ${tck-compare-strat-av-protocol} ${tck-compare-strat-av-protocol} strong-timed-bisim)
create_testcase(av-protocol-av-protocol-bisim ${tck-compare-strat-av-protocol} ${tck-compare-strat-av-protocol-bisim} strong-timed-bisim)
create_testcase(av-protocol-av-protocol-non-bisim-changed-guard ${tck-compare-strat-av-protocol} ${tck-compare-strat-av-protocol-non-bisim-changed-guard} strong-timed-bisim)
# create_testcase(av-protocol-av-protocol-non-bisim-changed-invariant ${tck-compare-strat-av-protocol} ${tck-compare-strat-av-protocol-non-bisim-changed-invariant} strong-timed-bisim)
# create_testcase(av-protocol-av-protocol-non-bisim-removed-reset ${tck-compare-strat-av-protocol} ${tck-compare-strat-av-protocol-non-bisim-removed-reset} strong-timed-bisim)

# # collision-avoidance benchmark variants
# create_testcase(collision-avoidance-self ${tck-compare-strat-collision-avoidance-bisim} ${tck-compare-strat-collision-avoidance-bisim} strong-timed-bisim)
# create_testcase(collision-avoidance-collision-avoidance-bisim ${tck-compare-strat-collision-avoidance-bisim} ${tck-compare-strat-collision-avoidance-bisim} strong-timed-bisim)
# create_testcase(collision-avoidance-collision-avoidance-non-bisim-changed-guard ${tck-compare-strat-collision-avoidance-bisim} ${tck-compare-strat-collision-avoidance-non-bisim-changed-guard} strong-timed-bisim)
# create_testcase(collision-avoidance-collision-avoidance-non-bisim-changed-invariant ${tck-compare-strat-collision-avoidance-bisim} ${tck-compare-strat-collision-avoidance-non-bisim-changed-invariant} strong-timed-bisim)
# create_testcase(collision-avoidance-collision-avoidance-non-bisim-removed-reset ${tck-compare-strat-collision-avoidance-bisim} ${tck-compare-strat-collision-avoidance-non-bisim-removed-reset} strong-timed-bisim)

# # ieee-rcp benchmark variants
# create_testcase(ieee-rcp-self ${tck-compare-strat-ieee-rcp} ${tck-compare-strat-ieee-rcp} strong-timed-bisim)
# create_testcase(ieee-rcp-ieee-rcp-bisim ${tck-compare-strat-ieee-rcp} ${tck-compare-strat-ieee-rcp-bisim} strong-timed-bisim)
# create_testcase(ieee-rcp-ieee-rcp-non-bisim-changed-guard ${tck-compare-strat-ieee-rcp} ${tck-compare-strat-ieee-rcp-non-bisim-changed-guard} strong-timed-bisim)
# create_testcase(ieee-rcp-ieee-rcp-non-bisim-changed-invariant ${tck-compare-strat-ieee-rcp} ${tck-compare-strat-ieee-rcp-non-bisim-changed-invariant} strong-timed-bisim)
# create_testcase(ieee-rcp-ieee-rcp-non-bisim-removed-reset ${tck-compare-strat-ieee-rcp} ${tck-compare-strat-ieee-rcp-non-bisim-removed-reset} strong-timed-bisim)

# urgent edge cases
create_testcase(urgent_self ${tck-compare-strat-urgent} ${tck-compare-strat-urgent} strong-timed-bisim)
create_testcase(urgent_non_urgent_self ${tck-compare-strat-urgent_non_urgent} ${tck-compare-strat-urgent_non_urgent} strong-timed-bisim)
create_testcase(urgent_vs_non_urgent ${tck-compare-strat-urgent} ${tck-compare-strat-urgent_non_urgent} strong-timed-bisim)

tck_add_savelist(save-compare-strategy ${savelist})
