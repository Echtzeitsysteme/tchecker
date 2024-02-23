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
    easy-ad94.sh:
    easy-ad94-added-transition.sh:
    easy-ad94-different-guard.sh:
    ad94.sh:
    Lieb_et_al_1.sh:
    Lieb_et_al_1_diff_invariant.sh:
    Lieb_et_al_2.sh:
    Lieb_et_al_3.sh:
    Lieb_et_al_4.sh:
    Lieb_et_al_5.sh:
    count_to_inf.sh:
    Lieb_et_al_1_non_determ_bisim.sh:
    Lieb_et_al_2_non_determ_bisim.sh:
    Lieb_et_al_2_determ_split_bisim.sh:
    Lieb_et_al_2_determ_split_non_bisim.sh:
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

list(GET TCK_COMPARE_INPUT_FILES 0 no_initial)
list(GET TCK_COMPARE_INPUT_FILES 1 easy-ad)
list(GET TCK_COMPARE_INPUT_FILES 2 easy-ad-added-transition)
list(GET TCK_COMPARE_INPUT_FILES 3 easy-ad-different-guard)
list(GET TCK_COMPARE_INPUT_FILES 4 ad)
list(GET TCK_COMPARE_INPUT_FILES 5 Lieb1)
list(GET TCK_COMPARE_INPUT_FILES 6 Lieb1_diff_inv)
list(GET TCK_COMPARE_INPUT_FILES 7 Lieb2)
list(GET TCK_COMPARE_INPUT_FILES 8 Lieb3)
list(GET TCK_COMPARE_INPUT_FILES 9 Lieb4)
list(GET TCK_COMPARE_INPUT_FILES 10 Lieb5)
list(GET TCK_COMPARE_INPUT_FILES 11 count_to_inf)
list(GET TCK_COMPARE_INPUT_FILES 12 Lieb1_non_determ_bisim)
list(GET TCK_COMPARE_INPUT_FILES 13 Lieb_et_al_2_non_determ_bisim)
list(GET TCK_COMPARE_INPUT_FILES 14 Lieb_et_al_2_determ_split_bisim)
list(GET TCK_COMPARE_INPUT_FILES 15 Lieb_et_al_2_determ_split_non_bisim)

# check whether the no_initial_check works
create_testcase(no_initial_first ${no_initial} ${ad} strong-timed-bisim)
create_testcase(no_initial_second ${ad} ${no_initial} strong-timed-bisim)

# testcases using easy-ad94

create_testcase(easy-ad94-self ${easy-ad} ${easy-ad} strong-timed-bisim)
create_testcase(easy-ad94-self-added-transition ${easy-ad-added-transition} ${easy-ad-added-transition} strong-timed-bisim)
create_testcase(easy-ad94-self-different-guard ${easy-ad-different-guard} ${easy-ad-different-guard} strong-timed-bisim)

create_testcase(easy-ad94-easy-ad94-added-transition ${easy-ad} ${easy-ad-added-transition} strong-timed-bisim)
create_testcase(easy-ad94-added-transition-easy-ad94 ${easy-ad-added-transition} ${easy-ad} strong-timed-bisim)

create_testcase(easy-ad94-different-guard-easy-ad94 ${easy-ad-different-guard} ${easy-ad} strong-timed-bisim)

create_testcase(easy-ad94-different-guard-easy-ad94-added-transition ${easy-ad-different-guard} ${easy-ad-added-transition} strong-timed-bisim)

# testcases using the examples of Lieb et al.

# Lieb1
create_testcase(Lieb-et-al-1-self ${Lieb1} ${Lieb1} strong-timed-bisim)

create_testcase(Lieb-et-al-1-Lieb-et-al-1-diff-inv ${Lieb1} ${Lieb1_diff_inv} strong-timed-bisim)
create_testcase(Lieb-et-al-1-Lieb-et-al-1-non-determ-bisim ${Lieb1} ${Lieb1_non_determ_bisim} strong-timed-bisim)

create_testcase(Lieb-et-al-1-Lieb-et-al-2 ${Lieb1} ${Lieb2} strong-timed-bisim)
create_testcase(Lieb-et-al-1-Lieb-et-al-3 ${Lieb1} ${Lieb3} strong-timed-bisim)
create_testcase(Lieb-et-al-1-Lieb-et-al-4 ${Lieb1} ${Lieb4} strong-timed-bisim)
create_testcase(Lieb-et-al-1-Lieb-et-al-5 ${Lieb1} ${Lieb5} strong-timed-bisim)

# Lieb2
create_testcase(Lieb-et-al-2-self ${Lieb2} ${Lieb2} strong-timed-bisim)

create_testcase(Lieb-et-al-2-Lieb-et-al-2-non-determ-bisim ${Lieb2} ${Lieb_et_al_2_non_determ_bisim} strong-timed-bisim)
create_testcase(Lieb-et-al-2-Lieb-et-al-2-determ-split-bisim ${Lieb2} ${Lieb_et_al_2_determ_split_bisim} strong-timed-bisim)
create_testcase(Lieb-et-al-2-Lieb-et-al-2-determ-split-non-bisim ${Lieb2} ${Lieb_et_al_2_determ_split_non_bisim} strong-timed-bisim)

create_testcase(Lieb-et-al-2-Lieb-et-al-3 ${Lieb2} ${Lieb3} strong-timed-bisim)
create_testcase(Lieb-et-al-2-Lieb-et-al-4 ${Lieb2} ${Lieb4} strong-timed-bisim)
create_testcase(Lieb-et-al-2-Lieb-et-al-5 ${Lieb2} ${Lieb5} strong-timed-bisim)

#Lieb 3
create_testcase(Lieb-et-al-3-self ${Lieb3} ${Lieb3} strong-timed-bisim)

create_testcase(Lieb-et-al-3-Lieb-et-al-4 ${Lieb3} ${Lieb4} strong-timed-bisim)
create_testcase(Lieb-et-al-3-Lieb-et-al-5 ${Lieb3} ${Lieb5} strong-timed-bisim)

#Lieb 4
create_testcase(Lieb-et-al-4-self ${Lieb4} ${Lieb4} strong-timed-bisim)

create_testcase(Lieb-et-al-4-Lieb-et-al-5 ${Lieb4} ${Lieb5} strong-timed-bisim)

#Lieb 5
create_testcase(Lieb-et-al-5-self ${Lieb5} ${Lieb5} strong-timed-bisim)

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

tck_add_savelist(save-compare ${savelist})
