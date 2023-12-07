# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(EXTRAPOLATION_SRC
${CMAKE_CURRENT_SOURCE_DIR}/extrapolation.cc
${CMAKE_CURRENT_SOURCE_DIR}/extrapolation_factory.cc
${CMAKE_CURRENT_SOURCE_DIR}/global_lu_extrapolation.cc
${CMAKE_CURRENT_SOURCE_DIR}/global_m_extrapolation.cc
${CMAKE_CURRENT_SOURCE_DIR}/k_norm.cc
${CMAKE_CURRENT_SOURCE_DIR}/local_lu_extrapolation.cc
${CMAKE_CURRENT_SOURCE_DIR}/local_m_extrapolation.cc
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/extrapolation.hh
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/extrapolation_factory.hh
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/global_lu_extrapolation.hh
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/global_m_extrapolation.hh
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/k_norm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/local_lu_extrapolation.hh
${TCHECKER_INCLUDE_DIR}/tchecker/extrapolation/local_m_extrapolation.hh
PARENT_SCOPE)
