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

no_extrapolation_t::no_extrapolation_t() : _max_value(0) { }

no_extrapolation_t::no_extrapolation_t(tchecker::ta::system_t const & system) {
  tchecker::zg::extrapolation_t * extrapolation{
      tchecker::zg::extrapolation_factory(tchecker::zg::EXTRA_M_GLOBAL, system)};
  _max_value = extrapolation->max_value();
  delete extrapolation;
}

void no_extrapolation_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) {}

tchecker::clockbounds::bound_t no_extrapolation_t::max_value() const {return _max_value;}

} // end of namespace zg

} // end of namespace tchecker
