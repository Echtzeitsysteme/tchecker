# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

# writing special register testcases function for tck-compare json
# this is neccessary since the original register testcases function accepts a single
# input model only

function(create_testcase TEST_NAME first_state second_state first_input second_input relationship)
    tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

    set_tests_properties(${TEST_NAME}
                         PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_COMPARE")

    tck_add_test_envvar(testenv TCK_COMPARE "${TCK_COMPARE}")
    tck_add_test_envvar(testenv TEST "${TCK_COMPARE_SH}")
    tck_add_test_envvar(testenv TEST_ARGS "-W -r ${relationship} --sFirst \\'${first_state}\\' --sSecond \\'${second_state}\\' ${first_input} ${second_input}")
    tck_add_test_envvar(testenv DOT_MAX_SIZE "${DOT_MAX_SIZE}")
    tck_set_test_env(${TEST_NAME} testenv)
    unset(testenv)
endfunction(create_testcase)

function(create_constraint_testcase TEST_NAME first_state second_state inter first_input second_input relationship)
    tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

    set_tests_properties(${TEST_NAME}
                         PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_COMPARE")

    tck_add_test_envvar(testenv TCK_COMPARE "${TCK_COMPARE}")
    tck_add_test_envvar(testenv TEST "${TCK_COMPARE_SH}")
    tck_add_test_envvar(testenv TEST_ARGS "-W -r ${relationship} --sFirst \\'${first_state}\\' --sSecond \\'${second_state}\\' --interConstraint ${inter} ${first_input} ${second_input}")
    tck_add_test_envvar(testenv DOT_MAX_SIZE "${DOT_MAX_SIZE}")
    tck_set_test_env(${TEST_NAME} testenv)
    unset(testenv)
endfunction(create_constraint_testcase)


option(TCK_ENABLE_COMPARE_JSON_TESTS "enable tests related to tck_compare with given states" ON)

if(NOT TCK_ENABLE_COMPARE_JSON_TESTS)
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
    easy-ad94.sh:
    easy-ad94-added-transition.sh:
    easy-ad94-different-guard.sh:
    Lieb_et_al_1.sh:
    Lieb_et_al_1_diff_invariant.sh:
    strong_timed_bisim_system_tests_edge_case_urgent.sh:
    strong_timed_bisim_system_tests_edge_case_urgent_non_urgent.sh:
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-compare-json-" CHECK_TESTCASES_ savelist TCK_COMPARE_INPUT_FILES ${INPUTS})

list(GET TCK_COMPARE_INPUT_FILES 0 easy-ad)
list(GET TCK_COMPARE_INPUT_FILES 1 easy-ad-added-transition)
list(GET TCK_COMPARE_INPUT_FILES 2 easy-ad-different-guard)
list(GET TCK_COMPARE_INPUT_FILES 3 Lieb1)
list(GET TCK_COMPARE_INPUT_FILES 4 Lieb1_diff_inv)
list(GET TCK_COMPARE_INPUT_FILES 5 edge_case_urgent)
list(GET TCK_COMPARE_INPUT_FILES 6 edge_case_urgent_non_urgent)

# define different start states

# start states for easy-ad94
set(TCK_COMPARE_JSON_EASY_AD_INIT_FULL_STATE '{"intval":"","labels":"","vloc":"<l0>","zone":"\(true\)"}')
set(TCK_COMPARE_JSON_EASY_AD_L_TWO_FULL_STATE '{"intval":"","labels":"","vloc":"<l2>","zone":"\(true\)"}')

set(TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE '{"intval":"","labels":"","vloc":"<l2>","zone":"\(2<x<3 && y==1 && 1<x-y<2\)"}')
set(TCK_COMPARE_JSON_EASY_AD_ADDED_TRANSITION_EXAMPLE_STATE '{"intval":"","labels":"","vloc":"<l1>","zone":"\(x<2 && y==1\)"}')
set(TCK_COMPARE_JSON_EASY_AD_DIFFERENT_GUARD_EXAMPLE_STATE '{"intval":"","labels":"","vloc":"<l1>","zone":"\(x<3 && y==1\)"}')

# constraints for easy-ad94
set(TCK_COMPARE_JSON_EASY_AD_INIT_CONSTRAINT '"\(2<x_1<3 && y_1==1 && 1<x_1-y_1<2 && 2<x_2<3 && y_2==1 && 1<x_2-y_2<2\)"')

# start states for Lieb-et-al-1
set(TCK_COMPARE_JSON_LIEB_1_START_STATE '{"intval":"","labels":"","vloc":"<A>","zone":"\(x==0\)"}')
set(TCK_COMPARE_JSON_LIEB_1_DIFF_INV_STATE '{"intval":"","labels":"","vloc":"<B>","zone":"\(x<2\)"}')

# start states for edge-case-urgent
set(TCK_COMPARE_JSON_URGENT_START_STATE '{"intval":"","labels":"","vloc":"<A>","zone":"\(x==0\)"}')
set(TCK_COMPARE_JSON_URGENT_FULL_STATE '{"intval":"","labels":"","vloc":"<A>","zone":"\(true\)"}')

# constraints for edge-case-urgent
set(TCK_COMPARE_JSON_URGENT_INIT_CONSTRAINT '"\(x_1==0 && x_2==0\)"')



# testcases using easy-ad94
## starting states
create_testcase(easy-ad94-self-json "${TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE}" "${TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE}" ${easy-ad} ${easy-ad} strong-timed-bisim)
create_testcase(easy-ad94-added-transition-json "${TCK_COMPARE_JSON_EASY_AD_ADDED_TRANSITION_EXAMPLE_STATE}" "${TCK_COMPARE_JSON_EASY_AD_ADDED_TRANSITION_EXAMPLE_STATE}" ${easy-ad} ${easy-ad-added-transition} strong-timed-bisim)
create_testcase(easy-ad94-different-guard-json "${TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE}" "${TCK_COMPARE_JSON_EASY_AD_DIFFERENT_GUARD_EXAMPLE_STATE}" ${easy-ad} ${easy-ad-different-guard} strong-timed-bisim)
## constraint
create_constraint_testcase(easy-ad-94-self-json-constraint "${TCK_COMPARE_JSON_EASY_AD_INIT_FULL_STATE}" "${TCK_COMPARE_JSON_EASY_AD_INIT_FULL_STATE}" "${TCK_COMPARE_JSON_EASY_AD_INIT_CONSTRAINT}" ${easy-ad} ${easy-ad} strong-timed-bisim)
create_constraint_testcase(easy-ad-94-self-json-constraint-second "${TCK_COMPARE_JSON_EASY_AD_L_TWO_FULL_STATE}" "${TCK_COMPARE_JSON_EASY_AD_L_TWO_FULL_STATE}" "${TCK_COMPARE_JSON_EASY_AD_INIT_CONSTRAINT}" ${easy-ad} ${easy-ad} strong-timed-bisim)

# testcases using the examples of Lieb et al.
create_testcase(Lieb-et-al-1-self-json "${TCK_COMPARE_JSON_LIEB_1_START_STATE}" "${TCK_COMPARE_JSON_LIEB_1_START_STATE}" ${Lieb1} ${Lieb1} strong-timed-bisim)
create_testcase(Lieb-et-al-1-Lieb-et-al-1-diff-inv-json "${TCK_COMPARE_JSON_LIEB_1_DIFF_INV_STATE}" "${TCK_COMPARE_JSON_LIEB_1_DIFF_INV_STATE}" ${Lieb1} ${Lieb1_diff_inv} strong-timed-bisim)

## urgent
create_testcase(edge-case-urgent-self-json "${TCK_COMPARE_JSON_URGENT_START_STATE}" "${TCK_COMPARE_JSON_URGENT_START_STATE}" ${edge_case_urgent} ${edge_case_urgent} strong-timed-bisim)
create_constraint_testcase(edge-case-urgent-self-json-second "${TCK_COMPARE_JSON_URGENT_FULL_STATE}" "${TCK_COMPARE_JSON_URGENT_FULL_STATE}" "${TCK_COMPARE_JSON_URGENT_INIT_CONSTRAINT}" ${edge_case_urgent} ${edge_case_urgent} strong-timed-bisim)

create_testcase(edge-case-urgent-vs-non-urgent-json "${TCK_COMPARE_JSON_URGENT_START_STATE}" "${TCK_COMPARE_JSON_URGENT_START_STATE}" ${edge_case_urgent} ${edge_case_urgent_non_urgent} strong-timed-bisim)

tck_add_savelist(save-compare-json ${savelist})
