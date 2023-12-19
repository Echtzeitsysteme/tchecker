# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(VCG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/vcg.cc
${CMAKE_CURRENT_SOURCE_DIR}/virtual_constraint.cc
${CMAKE_CURRENT_SOURCE_DIR}/zone.cc
${CMAKE_CURRENT_SOURCE_DIR}/zone_container.cc
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/vcg.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/virtual_constraint.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/zone.hh
${TCHECKER_INCLUDE_DIR}/tchecker/vcg/zone_container.hh
PARENT_SCOPE)
