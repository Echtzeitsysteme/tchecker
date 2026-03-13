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
    Lieb_et_al_1_non_determ_bisim.sh:
    strong_timed_bisim_system_tests_edge_case_changed_actions.sh:
    strong_timed_bisim_system_tests_edge_case_changed_actions_changed.sh:
    strong_timed_bisim_system_tests_edge_case_urgent.sh:
    strong_timed_bisim_system_tests_edge_case_urgent_non_urgent.sh:
    strong_timed_bisim_system_tests_edge_case_without_trans.sh:
    strong_timed_bisim_system_tests_edge_case_without_trans_changed_inv.sh:
    strong_timed_bisim_system_tests_path_explosion_A1.sh:
    strong_timed_bisim_system_tests_path_explosion_A2.sh:
    strong_timed_bisim_system_tests_path_explosion_A3.sh:
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-compare-json-" CHECK_TESTCASES_ savelist TCK_COMPARE_INPUT_FILES ${INPUTS})

list(GET TCK_COMPARE_INPUT_FILES 0 easy-ad)
list(GET TCK_COMPARE_INPUT_FILES 1 easy-ad-added-transition)
list(GET TCK_COMPARE_INPUT_FILES 2 easy-ad-different-guard)
list(GET TCK_COMPARE_INPUT_FILES 3 Lieb1)
list(GET TCK_COMPARE_INPUT_FILES 4 Lieb1_diff_inv)
list(GET TCK_COMPARE_INPUT_FILES 5 Lieb1_non_determ_bisim)
list(GET TCK_COMPARE_INPUT_FILES 6 edge_case_changed_actions)
list(GET TCK_COMPARE_INPUT_FILES 7 edge_case_changed_actions_changed)
list(GET TCK_COMPARE_INPUT_FILES 8 edge_case_urgent)
list(GET TCK_COMPARE_INPUT_FILES 9 edge_case_urgent_non_urgent)
list(GET TCK_COMPARE_INPUT_FILES 10 edge_case_without_trans)
list(GET TCK_COMPARE_INPUT_FILES 11 edge_case_without_trans_changed_inv)
list(GET TCK_COMPARE_INPUT_FILES 12 path_explosion_1)
list(GET TCK_COMPARE_INPUT_FILES 13 path_explosion_2)
list(GET TCK_COMPARE_INPUT_FILES 14 path_explosion_3)

# define different start states

# start states for easy-ad94
set(TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE '{"intval":"","labels":"","vloc":"<l2>","zone":"\(2<x<3 && y==1 && 1<x-y<2\)"}')
set(TCK_COMPARE_JSON_EASY_AD_ADDED_TRANSITION_EXAMPLE_STATE '{"intval":"","labels":"","vloc":"<l1>","zone":"\(x<2 && y==1\)"}')
set(TCK_COMPARE_JSON_EASY_AD_DIFFERENT_GUARD_EXAMPLE_STATE '{"intval":"","labels":"","vloc":"<l1>","zone":"\(x<3 && y==1\)"}')

# start states for Lieb-et-al-1
set(TCK_COMPARE_JSON_LIEB_1_START_STATE '{"intval":"","labels":"","vloc":"<A>","zone":"\(x==0\)"}')
set(TCK_COMPARE_JSON_LIEB_1_DIFF_INV_STATE '{"intval":"","labels":"","vloc":"<B>","zone":"\(x<2\)"}')

# testcases using easy-ad94
create_testcase(easy-ad94-self-json "${TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE}" "${TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE}" ${easy-ad} ${easy-ad} strong-timed-bisim)
create_testcase(easy-ad94-added-transition-json "${TCK_COMPARE_JSON_EASY_AD_ADDED_TRANSITION_EXAMPLE_STATE}" "${TCK_COMPARE_JSON_EASY_AD_ADDED_TRANSITION_EXAMPLE_STATE}" ${easy-ad} ${easy-ad-added-transition} strong-timed-bisim)
create_testcase(easy-ad94-different-guard-json "${TCK_COMPARE_JSON_EASY_AD_EXAMPLE_STATE}" "${TCK_COMPARE_JSON_EASY_AD_DIFFERENT_GUARD_EXAMPLE_STATE}" ${easy-ad} ${easy-ad-different-guard} strong-timed-bisim)

# testcases using the examples of Lieb et al.
create_testcase(Lieb-et-al-1-self-json "${TCK_COMPARE_JSON_LIEB_1_START_STATE}" "${TCK_COMPARE_JSON_LIEB_1_START_STATE}" ${Lieb1} ${Lieb1} strong-timed-bisim)
create_testcase(Lieb-et-al-1-Lieb-et-al-1-diff-inv-json "${TCK_COMPARE_JSON_LIEB_1_DIFF_INV_STATE}" "${TCK_COMPARE_JSON_LIEB_1_DIFF_INV_STATE}" ${Lieb1} ${Lieb1_diff_inv} strong-timed-bisim)
# create_testcase(Lieb-et-al-1-Lieb-et-al-1-non-determ-bisim ${Lieb1} ${Lieb1_non_determ_bisim} strong-timed-bisim)

# create_testcase(Lieb-et-al-1-Lieb-et-al-2 ${Lieb1} ${Lieb2} strong-timed-bisim)
# create_testcase(Lieb-et-al-1-Lieb-et-al-3 ${Lieb1} ${Lieb3} strong-timed-bisim)
# create_testcase(Lieb-et-al-1-Lieb-et-al-4 ${Lieb1} ${Lieb4} strong-timed-bisim)
# create_testcase(Lieb-et-al-1-Lieb-et-al-5 ${Lieb1} ${Lieb5} strong-timed-bisim)
# create_testcase(Lieb-et-al-1-Lieb-et-al-6 ${Lieb1} ${Lieb6} strong-timed-bisim)

# # Lieb2
# create_testcase(Lieb-et-al-2-self ${Lieb2} ${Lieb2} strong-timed-bisim)

# create_testcase(Lieb-et-al-2-Lieb-et-al-2-non-determ-bisim ${Lieb2} ${Lieb_et_al_2_non_determ_bisim} strong-timed-bisim)
# create_testcase(Lieb-et-al-2-Lieb-et-al-2-determ-split-bisim ${Lieb2} ${Lieb_et_al_2_determ_split_bisim} strong-timed-bisim)
# create_testcase(Lieb-et-al-2-Lieb-et-al-2-determ-split-non-bisim ${Lieb2} ${Lieb_et_al_2_determ_split_non_bisim} strong-timed-bisim)

# create_testcase(Lieb-et-al-2-Lieb-et-al-3 ${Lieb2} ${Lieb3} strong-timed-bisim)
# create_testcase(Lieb-et-al-2-Lieb-et-al-4 ${Lieb2} ${Lieb4} strong-timed-bisim)
# create_testcase(Lieb-et-al-2-Lieb-et-al-5 ${Lieb2} ${Lieb5} strong-timed-bisim)
# create_testcase(Lieb-et-al-2-Lieb-et-al-6 ${Lieb2} ${Lieb6} strong-timed-bisim)

# #Lieb 3
# create_testcase(Lieb-et-al-3-self ${Lieb3} ${Lieb3} strong-timed-bisim)

# create_testcase(Lieb-et-al-3-Lieb-et-al-4 ${Lieb3} ${Lieb4} strong-timed-bisim)
# create_testcase(Lieb-et-al-3-Lieb-et-al-5 ${Lieb3} ${Lieb5} strong-timed-bisim)
# create_testcase(Lieb-et-al-3-Lieb-et-al-6 ${Lieb3} ${Lieb6} strong-timed-bisim)

# #Lieb 4
# create_testcase(Lieb-et-al-4-self ${Lieb4} ${Lieb4} strong-timed-bisim)

# create_testcase(Lieb-et-al-4-Lieb-et-al-5 ${Lieb4} ${Lieb5} strong-timed-bisim)
# create_testcase(Lieb-et-al-4-Lieb-et-al-6 ${Lieb4} ${Lieb6} strong-timed-bisim)

# #Lieb 5
# create_testcase(Lieb-et-al-5-self ${Lieb5} ${Lieb5} strong-timed-bisim)
# create_testcase(Lieb-et-al-5-Lieb-et-al-6 ${Lieb5} ${Lieb6} strong-timed-bisim)

# # Lieb 6
# create_testcase(Lieb-et-al-6-self ${Lieb6} ${Lieb6} strong-timed-bisim)

# # edge cases

# ## changed actions
# create_testcase(edge-case-changed-actions-self ${edge_case_changed_actions} ${edge_case_changed_actions} strong-timed-bisim)
# create_testcase(edge-case-changed-actions-changed-self ${edge_case_changed_actions_changed} ${edge_case_changed_actions_changed} strong-timed-bisim)
# create_testcase(edge-case-changed-actions ${edge_case_changed_actions} ${edge_case_changed_actions_changed} strong-timed-bisim)

# ## urgent
# create_testcase(edge-case-urgent-self ${edge_case_urgent} ${edge_case_urgent} strong-timed-bisim)
# create_testcase(edge-case-urgent-non-urgent-self ${edge_case_urgent_non_urgent} ${edge_case_urgent_non_urgent} strong-timed-bisim)
# create_testcase(edge-case-urgent ${edge_case_urgent} ${edge_case_urgent_non_urgent} strong-timed-bisim)

# ## without transitions
# create_testcase(edge-case-without-transitions-self ${edge_case_without_trans} ${edge_case_without_trans} strong-timed-bisim)
# create_testcase(edge-case-without-transitions-changed-inv-self ${edge_case_without_trans_changed_inv} ${edge_case_without_trans_changed_inv} strong-timed-bisim)
# create_testcase(edge-case-without-transitions ${edge_case_without_trans} ${edge_case_without_trans_changed_inv} strong-timed-bisim)
# create_testcase(edge-case-without-transitions-Lieb-et-al-1 ${edge_case_without_trans} ${Lieb1} strong-timed-bisim)

# ## path explosion
# create_testcase(path-explosion-1-2 ${path_explosion_1} ${path_explosion_2} strong-timed-bisim)
# create_testcase(path-explosion-1-3 ${path_explosion_1} ${path_explosion_3} strong-timed-bisim)


# some self test take to long
#foreach(cur ${TCK_COMPARE_INPUT_FILES})
#
#  string(FIND ${cur} "/" last_slash REVERSE)
#  math(EXPR last_slash "${last_slash}+1")
#  string(SUBSTRING ${cur} ${last_slash} -1 test_name)
#  string(REPLACE ".out" "-self" test_name ${test_name})
#
#  if(NOT ${no_initial} STREQUAL ${cur})
#    create_testcase(${test_name} ${cur} ${cur} strong-timed-bisim)
#  endif()
#
#endforeach()

tck_add_savelist(save-compare-json ${savelist})
