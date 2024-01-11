/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_HH
#define TCHECKER_ZG_HH

#include "tchecker/zg/zg_template.hh"

/*!
 \file zg.hh
 \brief Zone graphs
 */

namespace tchecker {

namespace zg {

using zg_t = zg_help_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t, tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>;

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_HH
