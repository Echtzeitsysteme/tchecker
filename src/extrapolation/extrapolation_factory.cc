/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include "tchecker/extrapolation/extrapolation_factory.hh"

#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/clockbounds/solver.hh"
#include "tchecker/basictypes.hh"

#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/extrapolation/global_lu_extrapolation.hh"
#include "tchecker/extrapolation/global_m_extrapolation.hh"
#include "tchecker/extrapolation/local_lu_extrapolation.hh"
#include "tchecker/extrapolation/local_m_extrapolation.hh"


namespace tchecker {

namespace zg {

/* factories */

tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::ta::system_t const & system)
{
  if (extrapolation_type == tchecker::zg::NO_EXTRAPOLATION)
    return new tchecker::zg::no_extrapolation_t;

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds{tchecker::clockbounds::compute_clockbounds(system)};
  if (clock_bounds.get() == nullptr)
    return nullptr;

  return tchecker::zg::extrapolation_factory(extrapolation_type, *clock_bounds);
}

tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::clockbounds::clockbounds_t const & clock_bounds)
{
  if (extrapolation_type == tchecker::zg::NO_EXTRAPOLATION)
    return new tchecker::zg::no_extrapolation_t;
  switch (extrapolation_type) {
  case tchecker::zg::EXTRA_LU_GLOBAL:
    return new tchecker::zg::global_extra_lu_t{clock_bounds.global_lu_map()};
  case tchecker::zg::EXTRA_LU_LOCAL:
    return new tchecker::zg::local_extra_lu_t{clock_bounds.local_lu_map()};
  case tchecker::zg::EXTRA_LU_PLUS_GLOBAL:
    return new tchecker::zg::global_extra_lu_plus_t{clock_bounds.global_lu_map()};
  case tchecker::zg::EXTRA_LU_PLUS_LOCAL:
    return new tchecker::zg::local_extra_lu_plus_t{clock_bounds.local_lu_map()};
  case tchecker::zg::EXTRA_M_GLOBAL:
    return new tchecker::zg::global_extra_m_t{clock_bounds.global_m_map()};
  case tchecker::zg::EXTRA_M_LOCAL:
    return new tchecker::zg::local_extra_m_t{clock_bounds.local_m_map()};
  case tchecker::zg::EXTRA_M_PLUS_GLOBAL:
    return new tchecker::zg::global_extra_m_plus_t{clock_bounds.global_m_map()};
  case tchecker::zg::EXTRA_M_PLUS_LOCAL:
    return new tchecker::zg::local_extra_m_plus_t{clock_bounds.local_m_map()};
  default:
    throw std::invalid_argument("Unknown zone extrapolation");
  }
}

} // end of namespace zg

namespace vcg {

tchecker::zg::extrapolation_t * extrapolation_factory(
                  enum tchecker::zg::extrapolation_type_t type,
                  std::shared_ptr<const tchecker::ta::system_t> system_first,
                  std::shared_ptr<const tchecker::ta::system_t> system_second,
                  bool first_not_second, bool urgent_or_committed)
{

  if(tchecker::zg::EXTRA_M_GLOBAL != type && tchecker::zg::EXTRA_M_LOCAL != type) { // vcg currently support k norm (m_global) and local extrapolation (m_local) only
    throw std::invalid_argument("Unknown zone extrapolation");
  }

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds_first{tchecker::clockbounds::compute_clockbounds(*system_first)};
  if (clock_bounds_first.get() == nullptr)
    return nullptr;

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds_second{tchecker::clockbounds::compute_clockbounds(*system_second)};
  if (clock_bounds_second.get() == nullptr)
    return nullptr;

  tchecker::clock_id_t no_orig_clocks = (first_not_second) ? clock_bounds_first->clocks_number() : clock_bounds_second->clocks_number();

  auto map_size = no_orig_clocks + clock_bounds_first->clocks_number() + clock_bounds_second->clocks_number() + ((urgent_or_committed) ? 1 : 0);

  if(tchecker::zg::EXTRA_M_GLOBAL == type) { 
    
    std::shared_ptr<const tchecker::clockbounds::global_m_map_t> M_first = clock_bounds_first->global_m_map();
    std::shared_ptr<const tchecker::clockbounds::global_m_map_t> M_second = clock_bounds_second->global_m_map();

    tchecker::clockbounds::global_m_map_t m_map_with_virt_clks{map_size};

    tchecker::clockbounds::clear(m_map_with_virt_clks.M());

    /* since the virtual clock value must be the same as the original clock value in ab-synced states, we cannot extrapolate <=0 to <inf. Therefore, negative bounds must be removed */

    for(clock_id_t i = 0; i < no_orig_clocks; ++i) {
      tchecker::clockbounds::bound_t tmp = (first_not_second) ? M_first->M()[i] : M_second->M()[i];
      tchecker::clockbounds::update(m_map_with_virt_clks.M(), i, (0 > tmp) ? 0 : tmp);
    }

    for(clock_id_t i = 0; i < clock_bounds_first->clocks_number(); ++i) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(), i + no_orig_clocks, (0 > M_first->M()[i]) ? 0 : M_first->M()[i]);
    }

    for(clock_id_t i = 0; i < clock_bounds_second->clocks_number(); ++i) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(), i + no_orig_clocks + clock_bounds_first->clocks_number(), (0 > M_second->M()[i]) ? 0 : M_second->M()[i]);
    }

    if(urgent_or_committed) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(), no_orig_clocks + clock_bounds_first->clocks_number() + clock_bounds_second->clocks_number(), tchecker::clockbounds::NO_BOUND);
    }

    std::shared_ptr<tchecker::clockbounds::global_m_map_t const> final_map = std::make_shared<tchecker::clockbounds::global_m_map_t const>(m_map_with_virt_clks);

    return new tchecker::zg::global_extra_m_t{final_map};
  }

  std::shared_ptr<const tchecker::clockbounds::local_m_map_t> M_first = clock_bounds_first->local_m_map();
  std::shared_ptr<const tchecker::clockbounds::local_m_map_t> M_second = clock_bounds_second->local_m_map();

  tchecker::clockbounds::local_m_map_t m_map_with_virt_clks{clock_bounds_first->locations_number() + clock_bounds_second->locations_number(), map_size};

  /* One map for each location of first TA. Contains bounds for virtual clocks of first TA as well as bounds for original clocks of first TA only if it is the current TA. */
  for(loc_id_t j = 0; j < clock_bounds_first->locations_number(); ++j) {
    tchecker::clockbounds::clear(m_map_with_virt_clks.M(j));

    for(clock_id_t i = 0; i < no_orig_clocks; ++i) {
      tchecker::clockbounds::bound_t tmp = (first_not_second) ? M_first->M(j)[i] : 0;
      tchecker::clockbounds::update(m_map_with_virt_clks.M(j), i, (0 > tmp) ? 0 : tmp);
    }

    for(clock_id_t i = 0; i < clock_bounds_first->clocks_number(); ++i) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(j), i + no_orig_clocks, (0 > M_first->M(j)[i]) ? 0 : M_first->M(j)[i]);
    }

    for(clock_id_t i = 0; i < clock_bounds_second->clocks_number(); ++i) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(j), i + no_orig_clocks + clock_bounds_first->clocks_number(), 0);
    }

    if(urgent_or_committed) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(j), no_orig_clocks + clock_bounds_first->clocks_number() + clock_bounds_second->clocks_number(), tchecker::clockbounds::NO_BOUND);
    }
  }

  /* One map for each location of second TA. Contains bounds for virtual clocks of second TA as well as bounds for original clocks of second TA only if it is the current TA. */
  for(loc_id_t j = 0; j < clock_bounds_second->locations_number(); ++j) {
    tchecker::clockbounds::clear(m_map_with_virt_clks.M(clock_bounds_first->locations_number() + j));

    for(clock_id_t i = 0; i < no_orig_clocks; ++i) {
      tchecker::clockbounds::bound_t tmp = (first_not_second) ? 0 : M_second->M(j)[i];
      tchecker::clockbounds::update(m_map_with_virt_clks.M(clock_bounds_first->locations_number() + j), i, (0 > tmp) ? 0 : tmp);
    }

    for(clock_id_t i = 0; i < clock_bounds_first->clocks_number(); ++i) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(clock_bounds_first->locations_number() + j), i + no_orig_clocks, 0);
    }

    for(clock_id_t i = 0; i < clock_bounds_second->clocks_number(); ++i) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(clock_bounds_first->locations_number() + j), i + no_orig_clocks + clock_bounds_first->clocks_number(),  (0 > M_second->M(j)[i]) ? 0 : M_second->M(j)[i]);
    }

    if(urgent_or_committed) {
      tchecker::clockbounds::update(m_map_with_virt_clks.M(clock_bounds_first->locations_number() + j), no_orig_clocks + clock_bounds_first->clocks_number() + clock_bounds_second->clocks_number(), tchecker::clockbounds::NO_BOUND);
    }
  }

  std::shared_ptr<tchecker::clockbounds::local_m_map_t const> final_map = std::make_shared<tchecker::clockbounds::local_m_map_t const>(m_map_with_virt_clks);

  return new tchecker::zg::local_extra_m_t{final_map}; 

}

} // end of namespace vcg

} // end of namespace tchecker
