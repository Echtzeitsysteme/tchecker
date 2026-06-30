/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/contradiction_searcher.hh"
#include "tchecker/vcg/revert_transitions.hh"

namespace tchecker {

namespace strong_timed_bisim {

contradiction_searcher_t::contradiction_searcher_t(std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A, 
                                                   std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B,
                                                   tchecker::clock_id_t no_of_virt_clks)
                                                  : overhangs(trans_A->size(), trans_B->size(), no_of_virt_clks + 1),
                                                    no_of_virt_clks(no_of_virt_clks)
{
  for(size_t idx_A = 0; idx_A < trans_A->size(); idx_A++) {
    auto && [status_A, s_A, t_A] = (*trans_A)[idx_A];
    auto vc_A = tchecker::virtual_constraint::factory(s_A->zone(), no_of_virt_clks);
    
    for(size_t idx_B = 0; idx_B < trans_B->size(); idx_B++) {
      auto && [status_B, s_B, t_B] = (*trans_B)[idx_B];
      auto vc_B = tchecker::virtual_constraint::factory(s_B->zone(), no_of_virt_clks);

      auto overhang_A = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);
      auto overhang_B = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);
      
      vc_A->neg_logic_and(overhang_A, *vc_B);
      contradiction_t cont_A{overhang_A, 0};

      overhangs.add_contradiction(idx_A, idx_B, cont_A);

      vc_B->neg_logic_and(overhang_B, *vc_A);
      contradiction_t cont_B{overhang_B, 0};
      overhangs.add_contradiction(idx_A, idx_B, cont_B);
    }
  }
}

std::shared_ptr<tchecker::strong_timed_bisim::contradiction_t>
contradiction_searcher_t::search_contradiction(tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                               std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A, 
                                               std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B,
                                               tchecker::strong_timed_bisim::contradiction_matrix_t & found_cont)
{
  auto result = std::make_shared<tchecker::strong_timed_bisim::contradiction_t>(no_of_virt_clks);

  // find the contradictions for A
  for(size_t idx_A = 0; idx_A < trans_A->size(); idx_A++) {
    tchecker::integer_t current_steps = 0;
    contradiction_t new_cont{no_of_virt_clks, current_steps};

    auto && [status_A, s_A, t_A] = (*trans_A)[idx_A];
    auto cont_row = found_cont.get_row(idx_A);
    auto overhang_row = overhangs.get_row(idx_A);
    auto found = find_contradiction(s_A->zone(), trans_B, *cont_row, *overhang_row);
    for(auto cur : *found->get_contradictions()) {
      contradiction_t reverted{tchecker::vcg::revert_action_trans(zone_A, t_A->guard_container(), t_A->reset_container(), t_A->tgt_invariant_container(), *cur), found->min_steps_to_cont() + 1};
      new_cont.add_contradiction(reverted);
      current_steps = std::max(current_steps, reverted.min_steps_to_cont());
      new_cont.set_min_steps_to_cont(current_steps);
    }
    result->add_contradiction(new_cont);
  }

  // find the contradictions for B
  for(size_t idx_B = 0; idx_B < trans_B->size(); idx_B++) {
    tchecker::integer_t current_steps = 0;
    contradiction_t new_cont{no_of_virt_clks, current_steps};

    auto && [status_B, s_B, t_B] = (*trans_B)[idx_B];
    auto column = found_cont.get_column(idx_B);
    auto overhang_column = overhangs.get_column(idx_B);
    auto found = find_contradiction(s_B->zone(), trans_A, *column, *overhang_column);

    for(auto cur : *found->get_contradictions()) {
      contradiction_t reverted{tchecker::vcg::revert_action_trans(zone_B, t_B->guard_container(), t_B->reset_container(), t_B->tgt_invariant_container(), *cur), found->min_steps_to_cont() + 1};
      new_cont.add_contradiction(reverted);
      current_steps = std::max(current_steps, reverted.min_steps_to_cont());
      new_cont.set_min_steps_to_cont(current_steps);
    }
    result->add_contradiction(new_cont);
  }

  return result;
}

bool 
contradiction_searcher_t::contradiction_still_possible(tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                                       std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A,
                                                       std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B,
                                                       tchecker::strong_timed_bisim::contradiction_matrix_t & found_cont,
                                                       std::vector< std::vector<bool> > finished)
{

  assert(found_cont.get_no_of_rows() > 0);
  assert(found_cont.get_no_of_columns() > 0);

  assert(finished.size() == found_cont.get_no_of_rows());
  std::for_each(finished.begin(), finished.end(), [found_cont](std::vector<bool> cur) { assert(cur.size() == found_cont.get_no_of_columns()); } );

  tchecker::strong_timed_bisim::contradiction_matrix_t new_cont{found_cont.get_no_of_rows(), found_cont.get_no_of_columns(), found_cont.get_dim()};

  for(size_t i = 0; i < found_cont.get_no_of_rows(); ++i) {
    for(size_t j = 0; j < found_cont.get_no_of_columns(); ++j) {
      if(finished[i][j]) {
        std::shared_ptr<tchecker::strong_timed_bisim::contradiction_t> to_add = found_cont.get(i, j);
        new_cont.add_contradiction(i, j, *to_add);
      } else {
        contradiction_t empty_cont{no_of_virt_clks};
        new_cont.add_contradiction(i, j, empty_cont);
      }
    }
  }

  auto cont_possible = this->search_contradiction(zone_A, zone_B, trans_A, trans_B, new_cont);

  return !cont_possible->contradiction_free();

}

std::shared_ptr<tchecker::strong_timed_bisim::contradiction_t>
contradiction_searcher_t::find_contradiction(tchecker::zg::zone_t const & zone, std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans,
                                             std::vector<std::shared_ptr<tchecker::strong_timed_bisim::contradiction_t>> & found_con,
                                             std::vector<std::shared_ptr<tchecker::strong_timed_bisim::contradiction_t>> & cur_overhang)
{

  assert(0 != trans->size());
  assert(found_con.size() == trans->size());
  assert(cur_overhang.size() == trans->size());

  auto container = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);

  auto to_add = tchecker::virtual_constraint::factory(zone, no_of_virt_clks);

  // in the beginning, no subzone was ruled out. Therefore, we start with the whole zone.
  container->append_zone(to_add);

  for(unsigned int i = 0; i < trans->size(); ++i) {

    auto con = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clks + 1);

    con->append_container(found_con.at(i)->get_contradictions());
    con->append_container(cur_overhang.at(i)->get_contradictions());

    con->compress();

     // now we logically and the current overall contradiction with the new set of contradictions
    container = tchecker::logical_and_container<tchecker::virtual_constraint::virtual_constraint_t>(*container, *con, [] (tchecker::clock_id_t dim) {return tchecker::virtual_constraint::factory(dim - 1);} );

    container->compress();

  }

  tchecker::integer_t steps = 0;

  for(auto cur : found_con) {
    steps = std::max(steps, cur->min_steps_to_cont());
  }

  return std::make_shared<contradiction_t>(container, steps);
}


} // end of namespace strong_timed_bisim

} // end of namespace tchecker
