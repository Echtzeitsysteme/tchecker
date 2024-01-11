/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_STATE_HH
#define TCHECKER_ZG_STATE_HH

#include "tchecker/zg/state_template.hh"
#include "tchecker/zg/zone.hh"

#include "tchecker/utils/allocation_size.hh"

/*!
 \file state.hh
 \brief State of a zone graph
 */

namespace tchecker {

namespace zg {

using shared_zone_t = shared_zone_helper_t<tchecker::zg::zone_t>;

using zone_sptr_t = zone_sptr_helper_t<tchecker::zg::zone_t>;

using state_t = state_help_t<tchecker::zg::zone_t>;

using shared_state_t = shared_state_help_t<tchecker::zg::zone_t>;

using state_sptr_t = state_sptr_help_t<tchecker::zg::zone_t>;

using const_state_sptr_t = const_state_sptr_help_t<tchecker::zg::zone_t>;

} // end of namespace zg

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::zg::state_help_t<tchecker::zg::zone_t>
 */
template <> class allocation_size_t<tchecker::zg::state_help_t<tchecker::zg::zone_t>> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::zg::state_help_t
   \return allocation size for objects of class tchecker::zg::state_help_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args) { return sizeof(tchecker::zg::state_help_t<tchecker::zg::zone_t>); }
};

} // end of namespace tchecker

#endif // TCHECKER_ZG_STATE_HH
