/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/search_contradiction.hh"
#include "tchecker/vcg/revert_transitions.hh"

namespace tchecker {

namespace strong_timed_bisim {

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> find_contradiction_create_fill_up(tchecker::zg::zone_t const & zone, tchecker::virtual_constraint::virtual_constraint_t & other_side,
                                                                                                    std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vcs)
{

  auto inter = tchecker::virtual_constraint::factory(zone, vcs->dim() - 1);
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(vcs->dim());
  other_side.neg_logic_and(result, *inter);

  result->append_container(vcs);

  result->compress();

  return result;

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> 
find_contradiction(tchecker::zg::zone_t const & zone, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans,
                   std::vector<std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> & vcs, tchecker::clock_id_t no_of_virt_clks)
{

  assert(0 != trans.size());
  assert(vcs.size() == trans.size());

  auto result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);

  auto to_add = tchecker::virtual_constraint::factory(zone, no_of_virt_clks);

  result->append_zone(to_add);

  for(unsigned int i = 0; i < vcs.size(); ++i) {
    auto && [status, s, t] = *(trans[i]);
    auto vc_s = tchecker::virtual_constraint::factory(s->zone(), no_of_virt_clks);
    auto fill_up = find_contradiction_create_fill_up(zone, *vc_s, vcs[i]);

    result = tchecker::logical_and_container<tchecker::virtual_constraint::virtual_constraint_t>(*result, *fill_up, [] (tchecker::clock_id_t dim) {return tchecker::virtual_constraint::factory(dim - 1);} );
  }

  result->compress();
  result = tchecker::virtual_constraint::combine(*result, no_of_virt_clks);
  result->compress();

  return result;
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
search_contradiction(tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                     std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_A, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_B,
                     tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> found_cont, tchecker::clock_id_t no_of_virt_clks)
{
  auto contradiction = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);

  for(size_t idx_A = 0; idx_A < trans_A.size(); idx_A++) {
    auto && [status_A, s_A, t_A] = *(trans_A[idx_A]);
    auto row = found_cont.get_row(idx_A);
    auto found = find_contradiction(s_A->zone(), trans_B, *row, no_of_virt_clks);
    for(auto cur : *found) {
      contradiction->append_zone(tchecker::vcg::revert_action_trans(zone_A, t_A->guard_container(), t_A->reset_container(), t_A->tgt_invariant_container(), *cur));
    }
  }

  for(size_t idx_B = 0; idx_B < trans_B.size(); idx_B++) {
    auto && [status_B, s_B, t_B] = *(trans_B[idx_B]);
    auto column = found_cont.get_column(idx_B);
    auto found = find_contradiction(s_B->zone(), trans_A, *column, no_of_virt_clks);
    for(auto cur : *found) {
      contradiction->append_zone(tchecker::vcg::revert_action_trans(zone_B, t_B->guard_container(), t_B->reset_container(), t_B->tgt_invariant_container(), *cur));
    }
  }

  contradiction->compress();
  auto result = tchecker::virtual_constraint::combine(*contradiction, no_of_virt_clks);
  result->compress();

  return result;

}

} // end of namespace strong_timed_bisim

} // end of namespace tchecker
