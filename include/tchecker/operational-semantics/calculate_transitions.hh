/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OPERATIONAL_SEMANTICS_CALCULATE_TRANSITIONS_HH
#define TCHECKER_OPERATIONAL_SEMANTICS_CALCULATE_TRANSITIONS_HH

#include "tchecker/variables/clocks.hh"

/*!
 \file calculate_transition.hh
 \brief Transitions in Operational Semantics
 */

namespace tchecker {

namespace operational_semantics{

std::shared_ptr<tchecker::clockval_t> next(tchecker::clockval_t & clockval, tchecker::clock_reset_container_t const & clkreset);

} // end of namespace operational_semantics

} // end of namespace tchecker

#endif