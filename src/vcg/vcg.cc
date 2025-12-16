/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace vcg {

vcg_t::vcg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
             std::shared_ptr<tchecker::zg::semantics_t> const & semantics, tchecker::clock_id_t no_of_virtual_clocks, bool urgent_or_committed,
             std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size)
  : tchecker::zg::zg_t(system, sharing_type, semantics, extrapolation, block_size, table_size, false),
    _no_of_virtual_clocks(no_of_virtual_clocks), _urgent_or_committed(urgent_or_committed)
{
}

bool vcg_t::get_urgent_or_committed() const
{
  return _urgent_or_committed;
}

tchecker::clock_id_t vcg_t::get_no_of_virtual_clocks() const
{
  return _no_of_virtual_clocks;
}

tchecker::vcg::vcg_t * factory(std::shared_ptr<tchecker::strong_timed_bisim::system_virtual_clocks_t const> const & extended_system,
                               bool first_not_second,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_first,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_second,
                               bool urgent_or_committed,
                               enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                               enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                               std::size_t table_size) 
{

  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
    tchecker::vcg::extrapolation_factory(extrapolation_type, orig_system_first, orig_system_second, first_not_second, urgent_or_committed)};

  if (extrapolation.get() == nullptr)
    return nullptr;

  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};

  tchecker::clock_id_t vc = extended_system->get_no_of_virtual_clocks();

  return new tchecker::vcg::vcg_t(extended_system, sharing_type, semantics, vc, urgent_or_committed, extrapolation, block_size, table_size);
}

void vcg_t::avail_events(std::shared_ptr<std::set<std::set<std::string>>> result, tchecker::zg::state_sptr_t state)
{
  std::vector<tchecker::vcg::vcg_t::sst_t> v;
  tchecker::zg::const_state_sptr_t state_const{state};
  next(state_const, v);

  for (auto && [status, s, t] : v) {
    result->insert(t->vedge().event_names(system()));
  }
}


std::shared_ptr<tchecker::graph::edge_vedge_t> vcg_t::edge_of_event(tchecker::zg::state_sptr_t state, std::set<std::string> event)
{
  std::vector<tchecker::vcg::vcg_t::sst_t> v;
  tchecker::zg::const_state_sptr_t state_const{state};
  next(state_const, v);
  for (auto && [status, s, t] : v) {
    if(event == t->vedge().event_names(system())) {
      return std::make_shared<tchecker::graph::edge_vedge_t>(t->vedge_ptr());
    }
  }
  return nullptr;
}

void vcg_t::next_with_symbol(std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> result,
                             tchecker::zg::state_sptr_t state, std::set<std::string> symbol)
{

  std::vector<tchecker::vcg::vcg_t::sst_t> v;
  tchecker::zg::const_state_sptr_t state_const{state};
  next(state_const, v);

  for (tchecker::vcg::vcg_t::sst_t cur_trans : v) {
    assert(tchecker::dbm::is_tight(std::get<1>(cur_trans)->zone().dbm(), std::get<1>(cur_trans)->zone().dim()));
    if (std::get<2>(cur_trans)->vedge().event_equal(system(), symbol)) {
      result->emplace_back(cur_trans);
    }
  }
}

} // end of namespace vcg

} // end of namespace tchecker
