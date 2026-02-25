/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OPERATIONAL_ATTRIBUTES_TO_VALUATION_HH
#define TCHECKER_OPERATIONAL_ATTRIBUTES_TO_VALUATION_HH

#include <string>
#include <map>
#include "tchecker/variables/clocks.hh"

namespace tchecker {

namespace operational_semantics {

/*!
 \brief Generate clockval_t from attributes
 \param attributes : the attributes
 \return the clockval as described in attributes
 */
std::shared_ptr<tchecker::clockval_t> build(std::map<std::string, std::string> const & attributes);

} // end of namespace operational_semantics

} // end of namespace tchecker

#endif