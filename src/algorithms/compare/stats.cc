/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/algorithms/compare/stats.hh"

namespace tchecker {

namespace algorithms {

namespace compare {

stats_t::stats_t() : _visited_pair_of_states(0), _visited_transitions(0), _deepest_path(0), _relationship_fulfilled(true) {}

unsigned long stats_t::visited_pair_of_states() const { return _visited_pair_of_states; }

unsigned long stats_t::visited_transitions() const { return _visited_transitions; }

unsigned long stats_t::deepest_path() const { return _deepest_path; }

bool stats_t::relationship_fulfilled() const { return _relationship_fulfilled; }

void stats_t::attributes(std::map<std::string, std::string> & m) const {
  tchecker::algorithms::stats_t::attributes(m);

  std::stringstream sstream;

  sstream << _visited_pair_of_states;
  m["VISITED_PAIR_OF_STATES_STATES"] = sstream.str();

  sstream.str("");
  sstream << _visited_transitions;
  m["VISITED_TRANSITIONS"] = sstream.str();

  sstream.str("");
  sstream << _deepest_path;
  m["DEEPEST_PATH"] = sstream.str();

  sstream.str("");
  sstream << std::boolalpha << _relationship_fulfilled;
  m["RELATIONSHIP_FULFILLED"] = sstream.str();
}

} // end of namespace compare

} // end of namespace algorithms

} // end of namespace tchecker
