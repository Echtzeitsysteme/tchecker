/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/extrapolation/local_lu_extrapolation.hh"

namespace tchecker {

namespace zg {

namespace details {

local_lu_extrapolation_t::local_lu_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> const & clock_bounds)
    : _l(nullptr), _u(nullptr), _clock_bounds(clock_bounds)
{
  _l = tchecker::clockbounds::allocate_map(_clock_bounds->clock_number());
  _u = tchecker::clockbounds::allocate_map(_clock_bounds->clock_number());
}

local_lu_extrapolation_t::local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t const & e)
    : _clock_bounds(e._clock_bounds)
{
  _l = tchecker::clockbounds::clone_map(*e._l);
  _u = tchecker::clockbounds::clone_map(*e._u);
}

local_lu_extrapolation_t::local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t && e)
    : _l(std::move(e._l)), _u(std::move(e._u)), _clock_bounds(std::move(e._clock_bounds))
{
  e._l = nullptr;
  e._u = nullptr;
}

local_lu_extrapolation_t::~local_lu_extrapolation_t()
{
  tchecker::clockbounds::deallocate_map(_l);
  tchecker::clockbounds::deallocate_map(_u);
}

tchecker::zg::details::local_lu_extrapolation_t &
local_lu_extrapolation_t::operator=(tchecker::zg::details::local_lu_extrapolation_t const & e)
{
  if (this != &e) {
    _clock_bounds = e._clock_bounds;
    tchecker::clockbounds::deallocate_map(_l);
    _l = tchecker::clockbounds::clone_map(*e._l);
    tchecker::clockbounds::deallocate_map(_u);
    _u = tchecker::clockbounds::clone_map(*e._u);
  }
  return *this;
}

tchecker::zg::details::local_lu_extrapolation_t &
local_lu_extrapolation_t::operator=(tchecker::zg::details::local_lu_extrapolation_t && e)
{
  if (this != &e) {
    _clock_bounds = std::move(e._clock_bounds);
    _l = std::move(e._l);
    _u = std::move(e._u);
    e._l = nullptr;
    e._u = nullptr;
  }
  return *this;
}


} // end of namespace details

/* local_extra_lu_t */

void local_extra_lu_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_l, *_u);
  tchecker::dbm::extra_lu(dbm, dim, _l->ptr(), _u->ptr());
}

/* local_extra_lu_plus_t */

void local_extra_lu_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_l, *_u);
  tchecker::dbm::extra_lu_plus(dbm, dim, _l->ptr(), _u->ptr());
}

} // end of namespace zg

} // end of namespace tchecker
