/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/extrapolation/global_m_extrapolation.hh"

namespace tchecker {

namespace zg {

namespace details {

global_m_extrapolation_t::global_m_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::global_m_map_t const> const & clock_bounds)
    : _clock_bounds(clock_bounds)
{
}

tchecker::clockbounds::bound_t global_m_extrapolation_t::max_value() const
{
  tchecker::clockbounds::bound_t result = 0;

  for(tchecker::clockbounds::bound_t cur : this->_clock_bounds->M()) {
    if(cur > result) {
      result = cur;
    }
  }
  return result;
}

} // end of namespace details


/* global_extra_m_t */

void global_extra_m_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_m(dbm, dim, _clock_bounds->M().ptr());
}

/* global_extra_m_plus_t */

void global_extra_m_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_m_plus(dbm, dim, _clock_bounds->M().ptr());
}

} // end of namespace zg

} // end of namespace tchecker
