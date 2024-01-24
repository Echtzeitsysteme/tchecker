/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"
#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/vcg/revert_transitions.hh"
#include "tchecker/vcg/sync.hh"
#include "tchecker/dbm/dbm.hh"

namespace tchecker {

namespace strong_timed_bisim {

Lieb_et_al::Lieb_et_al(std::shared_ptr<tchecker::vcg::vcg_t> input_first, std::shared_ptr<tchecker::vcg::vcg_t> input_second)
  : _A(input_first), _B(input_first) {}

tchecker::strong_timed_bisim::stats_t Lieb_et_al::run() {

  std::cout << "run algorithm" << std::endl;

  stats_t stats;
#if 0
  stats.set_start_time();

  // sst is a tuple (tchecker::state_status_t, state_t, transition_t)
  // state_status_t : see basictypes.hh

  std::vector<tchecker::zg::zg_t::sst_t> sst_first;
  std::vector<tchecker::zg::zg_t::sst_t> sst_second;

  this->_A->initial(sst_first);
  this->_B->initial(sst_second);

  if(STATE_OK != std::get<0>(sst_first[0]) || STATE_OK != std::get<0>(sst_second[0])) 
    throw std::runtime_error("problems with initial state");

  std::size_t dim = std::get<1>(sst_first[0])->zone().dim();
  auto dbm = std::get<1>(sst_first[0])->zone().dbm();

  for(std::size_t i = 0; i < dim; ++i) {
      for(std::size_t j = 0; j < dim; ++j) {
        std::size_t index = i*dim + j;
        std::cout << "(" << dbm[index].cmp << ", " << dbm[index].value << ") ";
      }
      std::cout << std::endl;
  }

  tchecker::virtual_constraint::virtual_constraint_t *vc = tchecker::virtual_constraint::factory(&(std::get<1>(sst_first[0])->zone()), this->_A->get_no_of_virtual_clocks());

  const clock_constraint_container_t & virt_cons = vc->get_vc(this->_A->clocks_count() - this->_A->get_no_of_virtual_clocks());

  for(auto iter = virt_cons.begin(); iter < virt_cons.end(); iter++) {
    std::cout << *iter << std::endl;
  }
/*
  for(std::size_t i = 0; i <= input_first->get_no_of_virtual_clocks(); ++i) {
      for(std::size_t j = 0; j < input_first->get_no_of_virtual_clocks(); ++j) {
        std::size_t index = i*input_first->get_no_of_virtual_clocks() + i + j;
        std::cout << "(" << (vc->get_dbm())[index].cmp << ", " << (vc->get_dbm())[index].value << ") ";
      }
      std::cout << std::endl;
  }
*/
  delete vc;

  stats.set_end_time();
#endif
  return stats;

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_virt_bisim(tchecker::zg::const_state_sptr_t symb_state_first, tchecker::zg::transition_sptr_t symbolic_trans_first,
  tchecker::zg::const_state_sptr_t symb_state_second, tchecker::zg::transition_sptr_t symbolic_trans_second)
{

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  // we start with calculating D_A \land extract-virtual-constraint(D_B) (and the other way)
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_A = tchecker::virtual_constraint::factory(symb_state_first->zone(), _A->get_no_of_virtual_clocks());
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_B = tchecker::virtual_constraint::factory(symb_state_second->zone(), _A->get_no_of_virtual_clocks());

  tchecker::zg::state_sptr_t A_synced = _A->clone_state(symb_state_first);
  tchecker::zg::state_sptr_t B_synced = _B->clone_state(symb_state_second);

  // Before we sync them, we have to ensure virtual equivalence
  if(
    tchecker::dbm::status_t::EMPTY == phi_B->logic_and(A_synced->zone()) ||
    tchecker::dbm::status_t::EMPTY == phi_A->logic_and(B_synced->zone())
    )
  {
    // this is a difference to the original function, done for efficiency reasons.
    result->append_zone(phi_A);
    result->append_zone(phi_B);
    return result;
  }

  result->append_container(phi_A->logic_and(phi_B->neg()));
  result->append_container(phi_B->logic_and(phi_A->neg()));

  // now we can sync them

  tchecker::vcg::sync( A_synced->zone_ptr()->dbm(), B_synced->zone_ptr()->dbm(),
                       A_synced->zone_ptr()->dim(), B_synced->zone_ptr()->dim(),
                       _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                       symbolic_trans_first->reset_container(), symbolic_trans_second->reset_container());

  // normalizing, checking whether we have already seen this pair.
  tchecker::zg::state_sptr_t A_normed = _A->clone_state(A_synced);
  tchecker::zg::state_sptr_t B_normed = _B->clone_state(B_synced);

  _A->run_extrapolation(A_normed->zone().dbm(), A_normed->zone().dim(), *(A_normed->vloc_ptr()));
  _B->run_extrapolation(B_normed->zone().dbm(), B_normed->zone().dim(), *(B_normed->vloc_ptr()));

  std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> normalized_pair{A_normed, B_normed};

  if(_visited.count(normalized_pair)) {
    return result;
  }
  // If we haven't seen this pair, yet, add it to _visited
  _visited.insert(normalized_pair);

  // we first check the outgoing epsilon transition
  tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_synced);
  tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_synced);

  _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), symbolic_trans_first->tgt_invariant_container());
  _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), symbolic_trans_second->tgt_invariant_container());

  tchecker::zg::const_state_sptr_t const_A_epsilon{A_epsilon};
  tchecker::zg::const_state_sptr_t const_B_epsilon{B_epsilon};

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result_epsilon
    = check_for_virt_bisim(const_A_epsilon, symbolic_trans_first, const_B_epsilon, symbolic_trans_second);

  // now, we calculate the problematic virtual constraints by  using the revert-epsilon function and adding it to lo_not_simulatable
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> lo_not_simulatable
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  for(auto iter = result_epsilon->begin(); iter < result_epsilon->end(); iter++) {
    lo_not_simulatable->append_zone(tchecker::vcg::revert_epsilon_trans(A_synced->zone(), **iter));
  }

  tchecker::zg::const_state_sptr_t const_A_synced{A_synced};
  tchecker::zg::const_state_sptr_t const_B_synced{B_synced};

  // now that we have checked the epsilon transition, we check the outgoing action transitions
  lo_not_simulatable->append_container(check_for_outgoing_transitions_of_A(const_A_synced, const_B_synced));
  lo_not_simulatable->append_container(check_for_outgoing_transitions_of_B(const_A_synced, const_B_synced));

  // finally, we revert the sync

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> another_phi_A = tchecker::virtual_constraint::factory(symb_state_first->zone(), _A->get_no_of_virtual_clocks());
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> another_phi_B = tchecker::virtual_constraint::factory(symb_state_second->zone(), _A->get_no_of_virtual_clocks());

  tchecker::zg::state_sptr_t A_clone = _A->clone_state(symb_state_first);
  tchecker::zg::state_sptr_t B_clone = _B->clone_state(symb_state_second);

  another_phi_A->logic_and(B_clone->zone());
  another_phi_B->logic_and(A_clone->zone());

  tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> inter{_A->get_no_of_virtual_clocks() + 1};

  for(auto iter = lo_not_simulatable->begin(); iter < lo_not_simulatable->end(); iter++) {

    std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>> sync_reverted
      = tchecker::vcg::revert_sync(A_clone->zone_ptr()->dbm(), B_clone->zone_ptr()->dbm(), A_clone->zone_ptr()->dim(), B_clone->zone_ptr()->dim(),
                    _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                    **iter);
    inter.append_zone(sync_reverted.first);
    inter.append_zone(sync_reverted.second);
  }

  result->append_container(tchecker::virtual_constraint::combine(inter, _A->get_no_of_virtual_clocks()));

  return result;

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions( tchecker::zg::const_state_sptr_t symb_state_first,
                                            std::shared_ptr<tchecker::vcg::vcg_t> vcg_first,
                                            tchecker::zg::const_state_sptr_t symb_state_second,
                                            std::shared_ptr<tchecker::vcg::vcg_t> vcg_second)
{

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
     = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(vcg_first->get_no_of_virtual_clocks() + 1);

  std::vector<tchecker::vcg::vcg_t::sst_t> v_first;
  vcg_first->next(symb_state_first, v_first);

  for (auto && [status_first, s_first, t_first] : v_first) {

    std::vector<std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> lo_result;

    std::vector<tchecker::vcg::vcg_t::sst_t> v_second;
    vcg_second->next(symb_state_second, v_second);

    for (auto && [status_second, s_second, t_second] : v_second) {
      // the action has to be the same
      if(!t_first->vedge().event_equal(vcg_first->system(), t_second->vedge(), vcg_second->system())) {
        continue;
      }

      tchecker::zg::const_state_sptr_t const_s_first{s_first};
      tchecker::zg::const_state_sptr_t const_s_second{s_second};

      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> inter = this->check_for_virt_bisim(const_s_first, t_first, const_s_second, t_second);

      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> inter_land_s_first
        = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(vcg_first->get_no_of_virtual_clocks() + 1);

      for(auto iter = inter->begin(); iter < inter->end(); iter++) {
        std::shared_ptr<tchecker::zg::zone_t> tmp;
        if(tchecker::dbm::status_t::NON_EMPTY == (*iter)->logic_and(tmp, s_first->zone())) {
          inter_land_s_first->append_zone(tchecker::virtual_constraint::factory(tmp, vcg_first->get_no_of_virtual_clocks()));
        }
      }

      if(! inter_land_s_first->is_empty()) {
        lo_result.emplace_back(inter_land_s_first);
      }
    }

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> in_all =
      tchecker::virtual_constraint::contained_in_all(lo_result, vcg_first->get_no_of_virtual_clocks());

    for(auto iter = in_all->begin(); iter< in_all->end(); iter++) {
      if(!((*iter)->is_empty())) {
        result->append_zone(tchecker::vcg::revert_action_trans(s_first->zone(), t_first->guard_container(), t_first->reset_container(), t_first->tgt_invariant_container(), **iter));
      }
    }
  }

  return result;

}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

