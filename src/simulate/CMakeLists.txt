# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

add_subdirectory(symbolic)
add_subdirectory(concrete)

set(SIMULATE_SRC
${CMAKE_CURRENT_SOURCE_DIR}/simulate.cc
${TCHECKER_INCLUDE_DIR}/tchecker/simulate/simulate.hh
${SIMULATE_SYMBOLIC_SRC}
${SIMULATE_CONCRETE_SRC}
PARENT_SCOPE)