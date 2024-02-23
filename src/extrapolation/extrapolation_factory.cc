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
                  bool first_not_second)
{

  if(tchecker::zg::EXTRA_M_GLOBAL != type) { // vcg currently support k norm (m_global) only
    throw std::invalid_argument("Unknown zone extrapolation");
  }

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds_first{tchecker::clockbounds::compute_clockbounds(*system_first)};
  if (clock_bounds_first.get() == nullptr)
    return nullptr;

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds_second{tchecker::clockbounds::compute_clockbounds(*system_second)};
  if (clock_bounds_second.get() == nullptr)
    return nullptr;


  std::shared_ptr<const tchecker::clockbounds::global_m_map_t> M_first = clock_bounds_first->global_m_map();
  std::shared_ptr<const tchecker::clockbounds::global_m_map_t> M_second = clock_bounds_second->global_m_map();


  tchecker::clock_id_t no_orig_clocks = (first_not_second) ? clock_bounds_first->clocks_number() : clock_bounds_second->clocks_number();

  tchecker::clockbounds::global_m_map_t m_map_with_virt_clks{no_orig_clocks + clock_bounds_first->clocks_number() + clock_bounds_second->clocks_number()};

  tchecker::clockbounds::clear(m_map_with_virt_clks.M());

  for(clock_id_t i = 0; i < no_orig_clocks; ++i) {
    tchecker::clockbounds::update(m_map_with_virt_clks.M(), i, (first_not_second) ? M_first->M()[i] : M_second->M()[i]);
  }

  for(clock_id_t i = 0; i < clock_bounds_first->clocks_number(); ++i) {
    tchecker::clockbounds::update(m_map_with_virt_clks.M(), i + no_orig_clocks, M_first->M()[i]);
  }

  for(clock_id_t i = 0; i < clock_bounds_second->clocks_number(); ++i) {
    tchecker::clockbounds::update(m_map_with_virt_clks.M(), i + no_orig_clocks + clock_bounds_first->clocks_number(), M_second->M()[i]);
  }

  //std::cout << __FILE__ << ": " << __LINE__ << ": " << m_map_with_virt_clks << std::endl;

  std::shared_ptr<tchecker::clockbounds::global_m_map_t const> final_map = std::make_shared<tchecker::clockbounds::global_m_map_t const>(m_map_with_virt_clks);

  return new tchecker::zg::global_extra_m_t{final_map};

}

} // end of namespace vcg

} // end of namespace tchecker
