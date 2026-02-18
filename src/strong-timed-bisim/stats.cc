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

long stats_t::visited_pair_of_states() const
{
  return _visited_pair_of_states;
}

void stats_t::set_visited_pair_of_states(long visited_pair_of_states)
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

std::shared_ptr<tchecker::strong_timed_bisim::witness::graph_t> stats_t::witness() const
{
  return _witness;
}

void stats_t::init_witness(std::shared_ptr<tchecker::vcg::vcg_t> const & vcg1, 
                           std::shared_ptr<tchecker::vcg::vcg_t> const & vcg2)
{
  _witness = std::make_shared<tchecker::strong_timed_bisim::witness::graph_t>(vcg1, vcg2);
}

std::shared_ptr<tchecker::strong_timed_bisim::contra::cont_dag_t> stats_t::counterexample() const
{
  return _counterexample;
}


void stats_t::init_counterexample(std::shared_ptr<tchecker::vcg::vcg_t> const & vcg1, 
                         std::shared_ptr<tchecker::vcg::vcg_t> const & vcg2,
                         tchecker::zg::state_sptr_t first_init, tchecker::zg::state_sptr_t second_init,
                         std::size_t max_delay)
{
  _counterexample = std::make_shared<tchecker::strong_timed_bisim::contra::cont_dag_t>(vcg1, vcg2, first_init, second_init, max_delay);
}

void stats_t::attributes(std::map<std::string, std::string> & m) const {
  tchecker::algorithms::stats_t::attributes(m);

  std::stringstream sstream;

  sstream << _visited_pair_of_states;
  m["VISITED_PAIR_OF_STATES"] = sstream.str();

  sstream.str("");
  sstream << (_relationship_fulfilled ? "true" : "false");
  m["RELATIONSHIP_FULFILLED"] = sstream.str();
}

} // end of namespace algorithms

} // end of namespace tchecker
