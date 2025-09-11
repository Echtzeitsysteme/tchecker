/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

 #include "tchecker/strong-timed-bisim/certificate/witness/witness_node.hh"
 #include "tchecker/strong-timed-bisim/certificate/clock_names.hh"
 
namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

node_t::node_t(tchecker::zg::state_sptr_t const & s_1, tchecker::zg::state_sptr_t const & s_2,
               tchecker::clock_id_t no_of_virt_clks, std::size_t id, bool initial)
    : tchecker::strong_timed_bisim::certificate::node_t(s_1, s_2, initial)
{
  _zones = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);
  add_zone(tchecker::virtual_constraint::factory(s_1->zone(), no_of_virt_clks));
}

node_t::node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
               std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vc,
               std::size_t id, bool initial)
    : tchecker::strong_timed_bisim::certificate::node_t(location_pair, initial),
      _zones(vc)
{
}

node_t::node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, 
               std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vc, 
               std::size_t id, bool initial)
    : tchecker::strong_timed_bisim::certificate::node_t(first_loc, second_loc, initial),
      _zones(vc)
{
}

node_t::node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, 
         tchecker::clock_id_t no_of_virt_clks, std::size_t id, bool initial)
         : tchecker::strong_timed_bisim::certificate::node_t(first_loc, second_loc, initial)
{
  _zones = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);
}

std::size_t node_t::hash() const
{
  size_t h = tchecker::strong_timed_bisim::certificate::node_t::hash();
  boost::hash_combine(h, _zones->hash());

  return h;
}

void node_t::add_zone(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
{
  _zones->append_zone(vc);
  _zones->compress();
}

void node_t::add_zones(tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> &vcs)
{
  for (auto& cur : vcs) {
    add_zone(cur);
  }
}

bool node_t::empty() const {
  return std::all_of(_zones->begin(), _zones->end(), [](const auto& vc){ return vc->is_empty(); });
}

void node_t::compress() {
  _zones->compress();
}

void node_t::attributes(std::map<std::string, std::string> & m, const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                      const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
{
  bool first = true;

  std::ostringstream dbm_grep;
  for(auto cur : *(this->zones())) {
    if(!first) {
      dbm_grep << ", ";
    }
    tchecker::dbm::output(dbm_grep, cur->dbm(), cur->dim(), tchecker::strong_timed_bisim::certificate::clock_names(vcg1, vcg2));
    first = false;
  }

  m["zones"] = dbm_grep.str();

  tchecker::strong_timed_bisim::certificate::node_t::attributes(m, vcg1, vcg2);
}

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker