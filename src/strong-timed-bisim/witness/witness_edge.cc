/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/witness/witness_edge.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

edge_type_t::edge_type_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node_t> src,
               std::shared_ptr<node_t> tgt) :
    _edge_pair(std::make_shared<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>>(t1.vedge_ptr(), t2.vedge_ptr())),
    _src(src), _tgt(tgt)
{
}

bool edge_type_t::operator<(const edge_type_t& other) const
{
  auto cmp = tchecker::lexical_cmp(this->edge_pair_ptr()->first.vedge(), other.edge_pair_ptr()->first.vedge());
  if(0 != cmp) {
    return  cmp < 0;    
  }

  cmp = tchecker::lexical_cmp(this->edge_pair_ptr()->second.vedge(), other.edge_pair_ptr()->second.vedge());
  if(0 != cmp) {
    return  cmp < 0;    
  }

  cmp = this->_src->smaller_location_pair(*other._src);
  if(0 != cmp) {
    return cmp < 0;
  }

  cmp = this->_tgt->smaller_location_pair(*other._tgt);
  
  return cmp < 0;
}

bool edge_type_t::operator==(const edge_type_t& other) const
{
  return !(*this < other) && !(other < *this);
}

edge_t::edge_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node_t> src,
               std::shared_ptr<node_t> tgt, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition) :
               _type(std::make_shared<edge_type_t>(t1, t2, src, tgt)), _condition(condition) 
{
}

edge_t::edge_t(std::shared_ptr<edge_type_t> type, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition) :
               _type(type), _condition(condition) 
{
}

bool edge_t::is_subset(const edge_t& other) const {
  return this->_type == other._type &&
         *(this->condition()) <= *(other.condition());
}

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker