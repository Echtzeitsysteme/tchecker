# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(OPERATIONAL_SEMANTICS_SRC
${CMAKE_CURRENT_SOURCE_DIR}/max_delay.cc
${CMAKE_CURRENT_SOURCE_DIR}/zone_valuation_converter.cc
${TCHECKER_INCLUDE_DIR}/tchecker/operational-semantics/max_delay.hh
${TCHECKER_INCLUDE_DIR}/tchecker/operational-semantics/zone_valuation_converter.hh
PARENT_SCOPE)
