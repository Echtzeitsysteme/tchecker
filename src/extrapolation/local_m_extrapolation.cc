/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_LOCAL_M_EXTRAPOLATION_HH
#define TCHECKER_ZG_EXTRAPOLATION_LOCAL_M_EXTRAPOLATION_HH

#include "tchecker/extrapolation/local_m_extrapolation.hh"
#include "tchecker/extrapolation/extrapolation.hh"

namespace tchecker {

namespace zg {

namespace details {

local_m_extrapolation_t::local_m_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::local_m_map_t const> const & clock_bounds)
    : _m(nullptr), _clock_bounds(clock_bounds)
{
  _m = tchecker::clockbounds::allocate_map(_clock_bounds->clock_number());
}

local_m_extrapolation_t::local_m_extrapolation_t(tchecker::zg::details::local_m_extrapolation_t const & e)
    : _m(nullptr), _clock_bounds(e._clock_bounds)
{
  _m = tchecker::clockbounds::clone_map(*e._m);
}

local_m_extrapolation_t::local_m_extrapolation_t(tchecker::zg::details::local_m_extrapolation_t && e)
    : _m(std::move(e._m)), _clock_bounds(std::move(e._clock_bounds))
{
  e._m = nullptr;
}

local_m_extrapolation_t::~local_m_extrapolation_t() { tchecker::clockbounds::deallocate_map(_m); }

tchecker::zg::details::local_m_extrapolation_t &
local_m_extrapolation_t::operator=(tchecker::zg::details::local_m_extrapolation_t const & e)
{
  if (this != &e) {
    tchecker::clockbounds::deallocate_map(_m);
    _m = tchecker::clockbounds::clone_map(*e._m);
    _clock_bounds = e._clock_bounds;
  }
  return *this;
}

tchecker::zg::details::local_m_extrapolation_t &
local_m_extrapolation_t::operator=(tchecker::zg::details::local_m_extrapolation_t && e)
{
  if (this != &e) {
    _m = std::move(e._m);
    e._m = nullptr;
    _clock_bounds = std::move(e._clock_bounds);
  }
  return *this;
}

tchecker::clockbounds::bound_t local_m_extrapolation_t::max_value() const
{
  tchecker::clockbounds::bound_t result = 0;

  for (tchecker::loc_id_t loc = 0; loc < this->_clock_bounds->loc_number(); loc++) {
    for (tchecker::clockbounds::bound_t cur : this->_clock_bounds->M(loc)) {
      if (cur > result) {
        result = cur;
      }
    }
  }

  return result;
}

} // end of namespace details

/* local_extra_m_t */

void local_extra_m_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_m);
  tchecker::dbm::extra_m(dbm, dim, _m->ptr());
}

/* local_extra_m_plus_t */

void local_extra_m_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_m);
  tchecker::dbm::extra_m_plus(dbm, dim, _m->ptr());
}

} // enf of namespace zg

} // end of namespace tchecker

#endif
