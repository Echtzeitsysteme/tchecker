/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

 #include "tchecker/strong-timed-bisim/witness/witness_node.hh"
 
 #define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_NODE_SEED 0xDEADBEEF


namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

node_t::node_t(tchecker::zg::state_sptr_t const & s_1, tchecker::zg::state_sptr_t const & s_2, std::size_t id, tchecker::clock_id_t no_of_virt_clks)
    : _location_pair(std::make_shared<const std::pair<tchecker::ta::state_t, tchecker::ta::state_t>>(
                      tchecker::ta::state_t(s_1->vloc_ptr(), s_1->intval_ptr()), 
                      tchecker::ta::state_t(s_2->vloc_ptr(), s_2->intval_ptr()))),
      _zones(no_of_virt_clks),
      _id(id)
{
  add_zone(tchecker::virtual_constraint::factory(s_1->zone(), no_of_virt_clks));
}

bool node_t::smaller_location_pair(tchecker::strong_timed_bisim::witness::node_t const & other) const
{
  // we do not need to compare regarding the zones since each combination of TA states is unique
  int state_cmp = tchecker::ta::lexical_cmp((this->location_pair_ptr()->first), (other.location_pair_ptr()->first));
  if (state_cmp != 0)
    return (state_cmp < 0);
    
  state_cmp = tchecker::ta::lexical_cmp((this->location_pair_ptr()->second), (other.location_pair_ptr()->second));
  return (state_cmp < 0);
}

bool node_t::equal_location_pair(const node_t& other) const
{
  return !(this->smaller_location_pair(other)) && !(other.smaller_location_pair(*this));
}

std::size_t node_t::hash() const
{
  size_t h = TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_NODE_SEED;

  boost::hash_combine(h, tchecker::ta::hash_value(_location_pair->first));
  boost::hash_combine(h, tchecker::ta::hash_value(_location_pair->second));
  boost::hash_combine(h, _zones.hash());
  boost::hash_combine(h, _id);

  return h;
}

void node_t::add_zone(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
{
  _zones.append_zone(vc);
  _zones.compress();
}


} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker