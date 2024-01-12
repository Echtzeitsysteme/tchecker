# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(VCG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/revert_transitions.cc
${CMAKE_CURRENT_SOURCE_DIR}/sync.cc
${CMAKE_CURRENT_SOURCE_DIR}/vcg.cc
${CMAKE_CURRENT_SOURCE_DIR}/virtual_constraint.cc
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/revert_transitions.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/sync.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/vcg.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/virtual_constraint.hh
PARENT_SCOPE)
