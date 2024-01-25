/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/strong-timed-bisim/stats.hh"

namespace tchecker {

namespace strong_timed_bisim {

stats_t::stats_t() : _visited_pair_of_states(0), _relationship_fulfilled(true) {}

std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>::size_type stats_t::visited_pair_of_states() const
{
  return _visited_pair_of_states;
}

void stats_t::set_visited_pair_of_states(std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>::size_type visited_pair_of_states)
{
  _visited_pair_of_states = visited_pair_of_states;
}

bool stats_t::relationship_fulfilled() const {
  return _relationship_fulfilled;
}

void stats_t::set_relationship_fulfilled(bool relationship_fulfilled)
{
  _relationship_fulfilled = relationship_fulfilled;
}

void stats_t::attributes(std::map<std::string, std::string> & m) const {
  tchecker::algorithms::stats_t::attributes(m);

  std::stringstream sstream;

  sstream << _visited_pair_of_states;
  m["VISITED_PAIR_OF_STATES_STATES"] = sstream.str();

  sstream.str("");
  sstream << (_relationship_fulfilled ? "true" : "false");
  m["RELATIONSHIP_FULFILLED"] = sstream.str();
}

} // end of namespace algorithms

} // end of namespace tchecker
