/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OPERATIONAL_SEMANTICS_ZONE_VALUATION_CONVERTER_HH
#define TCHECKER_OPERATIONAL_SEMANTICS_ZONE_VALUATION_CONVERTER_HH

#include "tchecker/variables/clocks.hh"
#include "tchecker/zg/zone.hh"

namespace tchecker {

namespace operational_semantics {

/*!
 \brief Converts a zone into a clock valuation
 \param zone : the zone to convert
 \return a clock valuation from that zone
 */
std::shared_ptr<tchecker::clockval_t> convert(const tchecker::zg::zone_t & zone);

/*!
 \brief compresses the zone such that each value must be within a range of less than 1
 \param zone : the zone to compress
 \return EMPTY if the compressed zone is empty
 */
tchecker::dbm::status_t compress_zone(std::shared_ptr<tchecker::zg::zone_t> zone);

/*!
 \brief Converts a clock valuation into the corresponding region
 \param clockval : the valuation to convert
 \return a zone, representing the region of the clock valuation
 */
std::shared_ptr<tchecker::zg::zone_t> convert(std::shared_ptr<tchecker::clockval_t> clockval);

} // end of namespace operational_semantics

} // end of namespace tchecker

#endif