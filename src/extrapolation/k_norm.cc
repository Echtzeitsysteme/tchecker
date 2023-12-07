/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/extrapolation/k_norm.hh"

namespace tchecker {

namespace zg {

k_norm::k_norm(
    std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds)
    :tchecker::zg::global_extra_lu_t(clock_bounds)
{
  // k-norm is equivalent to global_extra_lu_t, if we always choose the maximum value of U(i) and L(i) for all checks.
  tchecker::clockbounds::map_t *U = const_cast<tchecker::clockbounds::map_t*>(&(_clock_bounds->U()));
  tchecker::clockbounds::map_t *L = const_cast<tchecker::clockbounds::map_t*>(& (_clock_bounds->L()));
  

  tchecker::clockbounds::update(*U, *L);
  tchecker::clockbounds::update(*L, *U);
}

} // end of namespace zg

namespace vcg {

k_norm_virtual::k_norm_virtual(
    std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds)
    :tchecker::zg::global_extra_lu_t(clock_bounds)
{
}

} // end of namespace vcg

} // end of namespace tchecker
