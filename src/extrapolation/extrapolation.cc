/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/extrapolation/extrapolation_factory.hh"
#include "tchecker/clockbounds/solver.hh"

namespace tchecker {

namespace zg {

/* no_extrapolation_t */

void no_extrapolation_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) {}

} // end of namespace zg

} // end of namespace tchecker
