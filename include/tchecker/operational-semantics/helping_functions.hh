/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OPERATIONAL_SEMANTICS_HELPING_FUNCTIONS_HH
#define TCHECKER_OPERATIONAL_SEMANTICS_HELPING_FUNCTIONS_HH

#include "tchecker/variables/clocks.hh"
#include "tchecker/zg/zone.hh"

namespace tchecker {

namespace operational_semantics {


/*!
 \brief find the maximum delay that stays in the zone
 \param zone : the zone that should not be left
 \param valuation : the valuation the delay should be added to
 \param max_delay_value : the maximum delay that should not be exceeded.
 \param min_delay_value : the minimum delay that should not be exceeded.
 \return the maximum delay that is lower or equals max_delay value and valuation + delay \in zone
 \note If valuation + max_delay_value \not\in zone and valuation + min_delay_value \not\in zone, 0 is returned.
 */
clock_rational_value_t
max_delay(tchecker::zg::zone_t & zone, tchecker::clockval_t * valuation, std::size_t max_delay_value, std::size_t min_delay_value);

} // end of namespace operational_semantics

} // end of namespace tchecker

#endif