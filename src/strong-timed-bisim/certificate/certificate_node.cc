/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/certificate_node.hh"

#define TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_NODE_SEED 0xDEADBEEF

namespace tchecker {

namespace strong_timed_bisim {

namespace certificate {

node_t::node_t(tchecker::zg::state_sptr_t const & s_1, tchecker::zg::state_sptr_t const & s_2,
               std::size_t id, bool initial)
    : _location_pair(std::make_shared<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>>(
                      tchecker::ta::state_t(s_1->vloc_ptr(), s_1->intval_ptr()), 
                      tchecker::ta::state_t(s_2->vloc_ptr(), s_2->intval_ptr()))),
      _id(id),
      _initial(initial)
{
}

node_t::node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
               std::size_t id, bool initial)
    : _location_pair(std::make_shared<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>>(
                      tchecker::ta::state_t(location_pair.first.vloc_ptr(), location_pair.first.intval_ptr()), 
                      tchecker::ta::state_t(location_pair.second.vloc_ptr(), location_pair.second.intval_ptr()))),
      _id(id),
      _initial(initial)
{
}

node_t::node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, 
               std::size_t id, bool initial)
    : _location_pair(std::make_shared<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>>(
                     tchecker::ta::state_t(first_loc.vloc_ptr(), first_loc.intval_ptr()), 
                     tchecker::ta::state_t(second_loc.vloc_ptr(), second_loc.intval_ptr()))),
      _id(id),
      _initial(initial)
{
}

bool node_t::operator<(node_t const & other) const
{
  int state_cmp = tchecker::ta::lexical_cmp((this->location_pair_ptr()->first), (other.location_pair_ptr()->first));
  if (state_cmp != 0)
    return (state_cmp < 0);
    
  state_cmp = tchecker::ta::lexical_cmp((this->location_pair_ptr()->second), (other.location_pair_ptr()->second));
  return (state_cmp < 0);
}

bool node_t::operator==(const node_t& other) const
{
  return !(*this<other) && !(other < (*this));
}

std::size_t node_t::hash() const
{
  size_t h = TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_NODE_SEED;

  boost::hash_combine(h, tchecker::ta::hash_value(_location_pair->first));
  boost::hash_combine(h, tchecker::ta::hash_value(_location_pair->second));
  boost::hash_combine(h, _id);

  return h;
}

void node_t::attributes(std::map<std::string, std::string> & m, 
                        const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                        const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
{
  m["initial"] = this->initial() ? "true" : "false";

  std::map<std::string, std::string> first_map;
  tchecker::ta::attributes(vcg1->system(), this->location_pair_ptr()->first, first_map);

  for (auto key : {std::string("intval"), std::string("vloc")}) {
    m["first_" + key] = first_map[key];
  }

  std::map<std::string, std::string> second_map;
  tchecker::ta::attributes(vcg2->system(), this->location_pair_ptr()->second, second_map);

  for (auto key : {std::string("intval"), std::string("vloc")}) {
    m["second_" + key] = second_map[key];
  }
}

} // end of namespace certificate

} // end of namespace strong_timed_bisim

} // end of namespace tchecker