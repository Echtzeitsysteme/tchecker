# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(COVREACH_SRC
${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
${CMAKE_CURRENT_SOURCE_DIR}/zg-covreach.cc
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/algorithm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/stats.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/zg-covreach.hh
PARENT_SCOPE)
