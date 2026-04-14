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

void extrapolation_t::extrapolate_synced(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc, 
                                         tchecker::clock_id_t no_original_clocks, tchecker::clock_id_t offset)
{
  this->extrapolate(dbm, dim, vloc);
  for(tchecker::clock_id_t i = 1; i <= no_original_clocks; ++i) {
    tchecker::dbm::constrain(dbm, dim, i, i+offset, tchecker::ineq_cmp_t::LE, 0);
    tchecker::dbm::constrain(dbm, dim, i+offset, i, tchecker::ineq_cmp_t::LE, 0);
  }
  tchecker::dbm::tighten(dbm, dim);
}

/* no_extrapolation_t */

no_extrapolation_t::no_extrapolation_t() : _max_value(0) { }

no_extrapolation_t::no_extrapolation_t(tchecker::ta::system_t const & system)
{
  tchecker::zg::extrapolation_t * extrapolation{
      tchecker::zg::extrapolation_factory(tchecker::zg::EXTRA_M_GLOBAL, system)};
  _max_value = extrapolation->max_value();
  delete extrapolation;
}

void no_extrapolation_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) {}

tchecker::clockbounds::bound_t no_extrapolation_t::max_value() const {return _max_value;}

} // end of namespace zg

} // end of namespace tchecker
