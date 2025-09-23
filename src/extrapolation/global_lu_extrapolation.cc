/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/extrapolation/global_lu_extrapolation.hh"

namespace tchecker {

namespace zg {

namespace details {

global_lu_extrapolation_t::global_lu_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds)
    : _clock_bounds(clock_bounds)
{
}

tchecker::clockbounds::bound_t global_lu_extrapolation_t::max_value() const
{
  tchecker::clockbounds::bound_t result = 0;

  for(tchecker::clockbounds::bound_t cur : this->_clock_bounds->L()) {
    if(cur > result) {
      result = cur;
    }
  }

  for(tchecker::clockbounds::bound_t cur : this->_clock_bounds->U()) {
    if(cur > result) {
      result = cur;
    }
  }

  return result;
}

} // end of namespace details

/* global_extra_lu_t */

void global_extra_lu_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_lu(dbm, dim, _clock_bounds->L().ptr(), _clock_bounds->U().ptr());
}

/* global_extra_lu_plus_t */

void global_extra_lu_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_lu_plus(dbm, dim, _clock_bounds->L().ptr(), _clock_bounds->U().ptr());
}

} // enf of namespace zg

} // end of namespace tchecker

