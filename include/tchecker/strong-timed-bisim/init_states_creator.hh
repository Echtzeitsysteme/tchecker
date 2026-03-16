/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

/*!
  \file init_states_creator.hh
  \brief create the initial states
*/

#ifndef TCHECKER_STRONG_TIMED_BISIM_INIT_STATES_CREATOR_HH
#define TCHECKER_STRONG_TIMED_BISIM_INIT_STATES_CREATOR_HH

#include<utility>
#include "tchecker/zg/zg.hh"
#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace strong_timed_bisim {

std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>
create_initial_states(const std::shared_ptr<tchecker::vcg::vcg_t> vcg_first,
                      const std::shared_ptr<tchecker::vcg::vcg_t> vcg_second,
                      std::map<std::string, std::string> & first_starting_state, std::vector<tchecker::zg::zg_t::sst_t> & sst_first,
                      std::map<std::string, std::string> & second_starting_state, std::vector<tchecker::zg::zg_t::sst_t> & sst_second,
                      std::string & inter_constraint);

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif