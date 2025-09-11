/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/witness/witness_edge.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

edge_t::edge_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node_t> source,
               std::shared_ptr<node_t> target, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition) :
               base_edge_t(t1, t2, source, target), _condition(condition) 
{
}

edge_t::edge_t(std::shared_ptr<base_edge_t> type, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition) :
               base_edge_t(*type), _condition(condition) 
{
}

bool edge_t::is_subset(const edge_t& other) const {
  return base_edge_t::operator==(other) &&
         *(this->condition()) <= *(other.condition());
}

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker