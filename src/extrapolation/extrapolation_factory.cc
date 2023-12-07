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
#include "tchecker/extrapolation/k_norm.hh"
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
  case tchecker::zg::EXTRA_K_NORM:
    return new tchecker::zg::k_norm{clock_bounds.global_lu_map()};
  default:
    throw std::invalid_argument("Unknown zone extrapolation");
  }
}

} // end of namespace zg

namespace vcg {

tchecker::zg::extrapolation_t * extrapolation_factory(
                  enum tchecker::zg::extrapolation_type_t type,
                  std::shared_ptr<tchecker::ta::system_t const> const & orig_system_first,
                  std::shared_ptr<tchecker::ta::system_t const> const & orig_system_second,
                  bool first_not_second)
{
  if(tchecker::zg::EXTRA_K_NORM != type)
    throw std::invalid_argument("vcg currently supports k normalization only.");

  std::vector<std::vector<tchecker::clockbounds::bound_t>> bounds;
  bounds.push_back(std::vector<tchecker::clockbounds::bound_t>());
  bounds.push_back(std::vector<tchecker::clockbounds::bound_t>());

  for(std::size_t i = 0; i < 2; ++i) {
    std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds{
      tchecker::clockbounds::compute_clockbounds(0 == i ? *orig_system_first : *orig_system_second)};

    if (clock_bounds.get() == nullptr)
      return nullptr;

    std::shared_ptr<tchecker::clockbounds::global_lu_map_t> lu_map = clock_bounds->global_lu_map();

      // we set the upper and lower clock bound to the same value to receive k-norm.
    tchecker::clockbounds::map_t *U = const_cast<tchecker::clockbounds::map_t*>(&(lu_map->U()));
    tchecker::clockbounds::map_t *L = const_cast<tchecker::clockbounds::map_t*>(& (lu_map->L()));

    tchecker::clockbounds::update(*U, *L);

    // ugly hack... TODO: make it nicer
    for(tchecker::clock_id_t j = 0; j < clock_bounds->clock_number(); ++j) {
      bounds[i].push_back(U[0][j]);
    }
  }

  tchecker::clock_id_t no_of_virtual_clocks = orig_system_first->clocks_count(VK_FLATTENED) + orig_system_second->clocks_count(VK_FLATTENED);
  tchecker::clock_id_t no_of_original_clocks = first_not_second ? orig_system_first->clocks_count(VK_FLATTENED) : orig_system_second->clocks_count(VK_FLATTENED);
  tchecker::clock_id_t no_of_clocks = no_of_original_clocks + no_of_virtual_clocks;

  std::shared_ptr<tchecker::clockbounds::global_lu_map_t> resulting_clock_bounds = std::make_shared<tchecker::clockbounds::global_lu_map_t>(no_of_clocks);

  /* set the bounds */

  // original
  for(tchecker::clock_id_t i = 0; i < no_of_original_clocks; ++i) {

    tchecker::clockbounds::bound_t bound = first_not_second ? bounds[0][i] : bounds[1][i];

    tchecker::clockbounds::update(resulting_clock_bounds->L(), i, bound);
    tchecker::clockbounds::update(resulting_clock_bounds->U(), i, bound);
  }


  // first virtual
  for(tchecker::clock_id_t i = no_of_original_clocks; i < no_of_original_clocks + orig_system_first->clocks_count(VK_FLATTENED); ++i) {
    tchecker::clockbounds::update(resulting_clock_bounds->L(), i, bounds[0][i - no_of_original_clocks]);
    tchecker::clockbounds::update(resulting_clock_bounds->U(), i, bounds[0][i - no_of_original_clocks]);
  }

  // second virtual
  tchecker::clock_id_t offset_second_virtual = no_of_original_clocks + orig_system_first->clocks_count(VK_FLATTENED);

  for(tchecker::clock_id_t i = offset_second_virtual; i < no_of_clocks; ++i) {
    tchecker::clockbounds::update(resulting_clock_bounds->L(), i, bounds[1][i - offset_second_virtual]);
    tchecker::clockbounds::update(resulting_clock_bounds->U(), i, bounds[1][i - offset_second_virtual]);
  }

  return new tchecker::vcg::k_norm_virtual{resulting_clock_bounds};

}

} // end of namespace vcg

} // end of namespace tchecker
