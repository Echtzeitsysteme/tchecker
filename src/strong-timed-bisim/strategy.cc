/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/strategy.hh"

namespace tchecker {

namespace strong_timed_bisim {

strategy_t::strategy_t(std::shared_ptr<tchecker::vcg::vcg_t> A, 
                       std::shared_ptr<tchecker::vcg::vcg_t> B,
                       std::vector<tchecker::zg::const_state_sptr_t> & symbolic_states_to_check)
  : _A(A), _B(B), _states_to_check(symbolic_states_to_check)
{
  assert(!symbolic_states_to_check.empty());
}

void strategy_t::insert_symb_states(std::shared_ptr<non_bisim_cache_t> non_bisim_cache, std::shared_ptr<visited_map_t> visited_map)
{
  _non_bisim_cache = non_bisim_cache;
  _visited_map = visited_map;
}

bool strategy_t::get_non_contained_states(std::pair<tchecker::zg::const_state_sptr_t, tchecker::zg::const_state_sptr_t> & result)
{
  throw std::runtime_error("Not Implemented Yet");
}

std::ostream & strategy_t::dot_output(std::ostream & os, std::string const & name)
{
  throw std::runtime_error("Not Implemented Yet");
}


} // end of namespace strong_timed_bisim

} // end of namespace tcheckers