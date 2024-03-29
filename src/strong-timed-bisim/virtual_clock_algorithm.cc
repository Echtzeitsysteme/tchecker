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
  : _A(input_first), _B(input_second), _visited_pair_of_states(0), _delete_me(0)
{
  assert(_A->get_no_of_virtual_clocks() == _B->get_no_of_virtual_clocks());
}

tchecker::strong_timed_bisim::stats_t Lieb_et_al::run() {

//  std::cout << __FILE__ << ": " << __LINE__ << ": no of orig clocks is " << _A->get_no_of_original_clocks() << " and " << _B->get_no_of_original_clocks() << std::endl;
//  std::cout << __FILE__ << ": " << __LINE__ << ": no of virt clocks is " << _A->get_no_of_virtual_clocks() << " and " << _B->get_no_of_virtual_clocks() << std::endl;

  tchecker::strong_timed_bisim::stats_t stats;

  stats.set_start_time();

  std::vector<tchecker::zg::zg_t::sst_t> sst_first;
  std::vector<tchecker::zg::zg_t::sst_t> sst_second;

  this->_A->initial(sst_first);
  this->_B->initial(sst_second);

  if(STATE_OK != std::get<0>(sst_first[0]) || STATE_OK != std::get<0>(sst_second[0])) 
    throw std::runtime_error("problems with initial state");

  tchecker::zg::const_state_sptr_t const_first{std::get<1>(sst_first[0])};
  tchecker::zg::const_state_sptr_t const_second{std::get<1>(sst_second[0])};

//  std::cout << __FILE__ << ": " << __LINE__ << ": start algorithm" << std::endl;

  std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> empty;

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result 
    = this->check_for_virt_bisim(const_first, std::get<2>(sst_first[0]), const_second, std::get<2>(sst_second[0]), empty, false);

  stats.set_end_time();

  stats.set_visited_pair_of_states(_visited_pair_of_states);
  stats.set_relationship_fulfilled(result->is_empty());

  return stats;

}

// used for assertion only
bool check_for_virt_bisim_preconditions_check(tchecker::zg::const_state_sptr_t symb_state, tchecker::zg::transition_sptr_t symb_trans)
{

  assert(tchecker::dbm::is_consistent(symb_state->zone().dbm(), symb_state->zone().dim()));
  assert(tchecker::dbm::is_tight(symb_state->zone().dbm(), symb_state->zone().dim()));

  // we check whether the resets of the zones are matching the resets of the transitions. WARNING: This works for reset to zero only!
  for(auto iter = symb_trans->reset_container().begin(); iter < symb_trans->reset_container().end(); iter++) {

    if(!iter->reset_to_zero()) {
      std::cerr << __FILE__ << ": " << __LINE__ << ": the reset " << *iter << " is not a reset to zero" << std::endl;
      return false;
    }

    tchecker::clock_constraint_t orig_min_ref{iter->left_id(), tchecker::REFCLOCK_ID, tchecker::LE, 0};
    tchecker::clock_constraint_t ref_min_orig{tchecker::REFCLOCK_ID, iter->left_id(), tchecker::LE, 0};

    if(!tchecker::dbm::satisfies(symb_state->zone().dbm(), symb_state->zone().dim(), orig_min_ref)) {
      std::cerr << __FILE__ << ": " << __LINE__ << ": the reset " << orig_min_ref << " is not fulfilled by" << std::endl;
      std::cerr << __FILE__ << ": " << __LINE__ << ": vloc: "<< symb_state->vloc() << std::endl;
      tchecker::dbm::output_matrix(std::cerr, symb_state->zone().dbm(), symb_state->zone().dim());
      return false;
    }

    if(!tchecker::dbm::satisfies(symb_state->zone().dbm(), symb_state->zone().dim(), ref_min_orig)) {
      std::cerr << __FILE__ << ": " << __LINE__ << ": the reset " << ref_min_orig << " is not fulfilled by" << std::endl;
      std::cerr << __FILE__ << ": " << __LINE__ << ": vloc: "<< symb_state->vloc() << std::endl;
      tchecker::dbm::output_matrix(std::cerr, symb_state->zone().dbm(), symb_state->zone().dim());
      return false;
    }
  }
  return true;
}

// used for assertion only
bool all_vc_are_sub_vc_of_phi_a_or_phi_b(tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & lo_vc, 
                                         tchecker::zg::const_state_sptr_t first, tchecker::zg::const_state_sptr_t second, tchecker::clock_id_t no_of_virt_clocks)
{
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_A = tchecker::virtual_constraint::factory(first->zone(), no_of_virt_clocks);
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_B = tchecker::virtual_constraint::factory(second->zone(), no_of_virt_clocks);
  bool result = true;

  for(auto iter = lo_vc.begin(); iter < lo_vc.end(); iter++) {

    assert(tchecker::dbm::is_tight((*iter)->dbm(), (*iter)->dim()));
    assert(tchecker::dbm::is_consistent((*iter)->dbm(), (*iter)->dim()));

    result &= ((*iter)->dim() == no_of_virt_clocks + 1);
    result &= (tchecker::dbm::is_le((*iter)->dbm(), phi_A->dbm(), phi_A->dim()) || tchecker::dbm::is_le((*iter)->dbm(), phi_B->dbm(), phi_B->dim()));

    if(!result) {
      std::cout << __FILE__ << ": " << __LINE__ << ": problems with the return of check_for_virt_bisim" << std::endl << "phi_A is " << std::endl;
      tchecker::dbm::output_matrix(std::cout, phi_A->dbm(), phi_A->dim());

      std::cout << __FILE__ << ": " << __LINE__ << ": phi_B is " << std::endl;
      tchecker::dbm::output_matrix(std::cout, phi_B->dbm(), phi_B->dim());

      std::cout << __FILE__ << ": " << __LINE__ << ": returned vc is " << std::endl;
      tchecker::dbm::output_matrix(std::cout, (*iter)->dbm(), (*iter)->dim());
      return result;
    }
  }
  return result;
}

// used for assertion only
bool is_phi_subset_of_a_zone(const tchecker::dbm::db_t *dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virt_clocks,
                             const tchecker::virtual_constraint::virtual_constraint_t & phi_e)
{
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi = tchecker::virtual_constraint::factory(dbm, dim, no_of_virt_clocks);

  return tchecker::dbm::is_le(phi_e.dbm(), phi->dbm(), phi_e.dim());
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_virt_bisim(tchecker::zg::const_state_sptr_t symb_state_first, tchecker::zg::transition_sptr_t symb_trans_first,
                                 tchecker::zg::const_state_sptr_t symb_state_second, tchecker::zg::transition_sptr_t symb_trans_second,
                                 std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited,
                                 bool last_was_epsilon)
{

  if(!last_was_epsilon) {
    assert(check_for_virt_bisim_preconditions_check(symb_state_first, symb_trans_first));
    assert(check_for_virt_bisim_preconditions_check(symb_state_second, symb_trans_second));
  }

  _visited_pair_of_states++;

  //std::cout << __FILE__ << ": " << __LINE__ << ": _visited_pair_of_states: " << _visited_pair_of_states << std::endl;
  //std::cout << __FILE__ << ": " << __LINE__ << ": check-for-virt-bisim" << std::endl;
  //std::cout << __FILE__ << ": " << __LINE__ << ": " << symb_state_first->vloc() << std::endl;
  //tchecker::dbm::output_matrix(std::cout, symb_state_first->zone().dbm(), symb_state_first->zone().dim());

  //std::cout << __FILE__ << ": " << __LINE__ << ": " << symb_state_second->vloc() << std::endl;
  //tchecker::dbm::output_matrix(std::cout, symb_state_second->zone().dbm(), symb_state_second->zone().dim());

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_A = tchecker::virtual_constraint::factory(symb_state_first->zone(), _A->get_no_of_virtual_clocks());
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_B = tchecker::virtual_constraint::factory(symb_state_second->zone(), _A->get_no_of_virtual_clocks());

  tchecker::zg::state_sptr_t A_synced = _A->clone_state(symb_state_first);
  tchecker::zg::state_sptr_t B_synced = _B->clone_state(symb_state_second);

  // Before we sync them, we have to ensure virtual equivalence
  if(
    tchecker::dbm::status_t::EMPTY == phi_B->logic_and(A_synced->zone(), symb_state_first->zone()) ||
    tchecker::dbm::status_t::EMPTY == phi_A->logic_and(B_synced->zone(), symb_state_second->zone())
    )
  {
    // this is a difference to the original function, done for efficiency reasons.
    result->append_zone(phi_A);
    result->append_zone(phi_B);
    assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*result, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));
    _delete_me++;
    //std::cout << __FILE__ << ": " << __LINE__ << ": _delete_me: " << _delete_me << std::endl;
    result->compress();
    return result;
  }

  //std::cout << __FILE__ << ": " << __LINE__ << ": " << "virt_equiv A:" << std::endl;
  //tchecker::dbm::output_matrix(std::cout, A_synced->zone().dbm(), A_synced->zone().dim());
  //std::cout << __FILE__ << ": " << __LINE__ << ": " << "virt_equiv B:" << std::endl;
  //tchecker::dbm::output_matrix(std::cout, B_synced->zone().dbm(), B_synced->zone().dim());

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_append_A 
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_append_B 
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_B->get_no_of_virtual_clocks() + 1);

  phi_B->neg_logic_and(to_append_A, *phi_A);
  phi_A->neg_logic_and(to_append_B, *phi_B);

  result->append_container(to_append_A);
  result->append_container(to_append_B);

  assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*result, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));

  // now we can sync them. As we know: the targets of delay transitions are already synced!
  if(!last_was_epsilon) {
    tchecker::vcg::sync( A_synced->zone_ptr()->dbm(), B_synced->zone_ptr()->dbm(),
                         A_synced->zone_ptr()->dim(), B_synced->zone_ptr()->dim(),
                         _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                         symb_trans_first->reset_container(), symb_trans_second->reset_container());

    //std::cout << __FILE__ << ": " << __LINE__ << ": " << "synced A:" << std::endl;
    //tchecker::dbm::output_matrix(std::cout, A_synced->zone().dbm(), A_synced->zone().dim());
    //std::cout << __FILE__ << ": " << __LINE__ << ": " << "synced B:" << std::endl;
    //tchecker::dbm::output_matrix(std::cout, B_synced->zone().dbm(), B_synced->zone().dim());
  }

  assert(tchecker::vcg::are_dbm_synced(A_synced->zone_ptr()->dbm(), B_synced->zone_ptr()->dbm(),
                                       A_synced->zone_ptr()->dim(), B_synced->zone_ptr()->dim(),
                                       _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks()));

  // normalizing, checking whether we have already seen this pair.
  tchecker::zg::state_sptr_t A_normed = _A->clone_state(A_synced);
  tchecker::zg::state_sptr_t B_normed = _B->clone_state(B_synced);

  _A->run_extrapolation(A_normed->zone().dbm(), A_normed->zone().dim(), *(A_normed->vloc_ptr()));
  _B->run_extrapolation(B_normed->zone().dbm(), B_normed->zone().dim(), *(B_normed->vloc_ptr()));

  std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> normalized_pair{A_normed, B_normed};

  if(visited.count(normalized_pair)) {
    assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*result, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));
    _delete_me++;
    //std::cout << __FILE__ << ": " << __LINE__ << ": _delete_me: " << _delete_me << std::endl;
    result->compress();
    return result;
  }

  // If we haven't seen this pair, yet, add it to visited
  visited.insert(normalized_pair);

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> lo_not_simulatable
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  if(!last_was_epsilon) {
    // we check the outgoing epsilon transition
    tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_normed);
    tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_normed);

    _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), symb_trans_first->tgt_invariant_container());
    _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), symb_trans_second->tgt_invariant_container());

    tchecker::zg::const_state_sptr_t const_A_epsilon{A_epsilon};
    tchecker::zg::const_state_sptr_t const_B_epsilon{B_epsilon};

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result_epsilon
      = check_for_virt_bisim(const_A_epsilon, symb_trans_first, const_B_epsilon, symb_trans_second, visited, true);

    // now, we calculate the problematic virtual constraints by using the revert-epsilon function and adding it to lo_not_simulatable

    for(auto iter = result_epsilon->begin(); iter < result_epsilon->end(); iter++) {
      lo_not_simulatable->append_zone(tchecker::vcg::revert_epsilon_trans(A_normed->zone(), A_epsilon->zone(), **iter));
      lo_not_simulatable->append_zone(tchecker::vcg::revert_epsilon_trans(B_normed->zone(), B_epsilon->zone(), **iter));
    }

  }
 else {

    tchecker::zg::const_state_sptr_t const_A_normed{A_normed};
    tchecker::zg::const_state_sptr_t const_B_normed{B_normed};

    // now that we have checked the epsilon transition, we check the outgoing action transitions
    lo_not_simulatable->append_container(check_for_outgoing_transitions(const_A_normed, const_B_normed, visited));
  }

  lo_not_simulatable->compress();

  // now we have to revert the extrapolation

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> reverted_extrapolation
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_synced = tchecker::virtual_constraint::factory(A_synced->zone(), _A->get_no_of_virtual_clocks()); // vc of A_synced and B_synced are the same

  for(auto iter = lo_not_simulatable->begin(); iter < lo_not_simulatable->end(); iter++) {
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_Add = tchecker::virtual_constraint::factory(_A->get_no_of_virtual_clocks());
    if(tchecker::dbm::NON_EMPTY == tchecker::dbm::intersection(to_Add->dbm(), (*iter)->dbm(), phi_synced->dbm(), _A->get_no_of_virtual_clocks() + 1)) {
      reverted_extrapolation->append_zone(to_Add);
    }
  }

  reverted_extrapolation->compress();

  // finally, we revert the sync

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> another_phi_A = tchecker::virtual_constraint::factory(symb_state_first->zone(), _A->get_no_of_virtual_clocks());
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> another_phi_B = tchecker::virtual_constraint::factory(symb_state_second->zone(), _B->get_no_of_virtual_clocks());

  tchecker::zg::state_sptr_t A_clone = _A->clone_state(symb_state_first);
  tchecker::zg::state_sptr_t B_clone = _B->clone_state(symb_state_second);

  another_phi_A->logic_and(B_clone->zone(), symb_state_second->zone());
  another_phi_B->logic_and(A_clone->zone(), symb_state_first->zone());

  tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> inter{_A->get_no_of_virtual_clocks() + 1};

  for(auto iter = reverted_extrapolation->begin(); iter < reverted_extrapolation->end(); iter++) {
    std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>> sync_reverted
      = tchecker::vcg::revert_sync(A_clone->zone_ptr()->dbm(), B_clone->zone_ptr()->dbm(), A_clone->zone_ptr()->dim(), B_clone->zone_ptr()->dim(),
                    _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                    **iter);

    if(sync_reverted.first->is_fulfillable()) {
      inter.append_zone(sync_reverted.first);
      assert(is_phi_subset_of_a_zone(symb_state_first->zone().dbm(), symb_state_first->zone().dim(), _A->get_no_of_virtual_clocks(), *(sync_reverted.first)));
    }

    if(sync_reverted.second->is_fulfillable()) {
      inter.append_zone(sync_reverted.second);
      assert(is_phi_subset_of_a_zone(symb_state_second->zone().dbm(), symb_state_second->zone().dim(), _B->get_no_of_virtual_clocks(), *(sync_reverted.second)));
    }

  }

  inter.compress();

  assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(inter, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));

  result->append_container(tchecker::virtual_constraint::combine(inter, _A->get_no_of_virtual_clocks()));

  assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*result, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));

  _delete_me++;
  //std::cout << __FILE__ << ": " << __LINE__ << ": _delete_me: " << _delete_me << std::endl;

  result->compress();

  return result;
}


std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions( tchecker::zg::const_state_sptr_t A_state,
                                            tchecker::zg::const_state_sptr_t B_state,
                                            std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
     = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  std::vector<tchecker::vcg::vcg_t::sst_t> v_first, v_second;
  _A->next(A_state, v_first);
  _B->next(B_state, v_second);

  // vector of pointer to zone_container to store the return values regarding an outgoing transition
  typedef std::vector<std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> return_values;

  // vector with an entry for each outgoing transition. For each outgoing trans, a vector of zone container is stored
  typedef std::vector<return_values> lo_return_values; 

  lo_return_values A_return_values;

  lo_return_values B_return_values;


  std::tuple<std::vector<tchecker::vcg::vcg_t::sst_t> *, lo_return_values *, tchecker::zg::const_state_sptr_t *> A_v_ret_val_pair
                      = std::make_tuple<std::vector<tchecker::vcg::vcg_t::sst_t> *, lo_return_values *, tchecker::zg::const_state_sptr_t *>(&v_first, &A_return_values, &A_state);

  std::tuple<std::vector<tchecker::vcg::vcg_t::sst_t> *, lo_return_values *, tchecker::zg::const_state_sptr_t *> B_v_ret_val_pair
                      = std::make_tuple<std::vector<tchecker::vcg::vcg_t::sst_t> *, lo_return_values *, tchecker::zg::const_state_sptr_t *>(&v_second, &B_return_values, &B_state);

  auto together = {&A_v_ret_val_pair, &B_v_ret_val_pair};

  // init the return values of each outgoing transition.
  // for A and B
  for(auto iter : together) {
    auto ret_val = std::get<1>(*iter);
    // and for each outgoing transition
    for(auto && [status, s, t] : *(std::get<0>(*iter))) {
      // create a zone container
      tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> tmp{_A->get_no_of_virtual_clocks() + 1};
      // that contains the target vc only
      tmp.append_zone(tchecker::virtual_constraint::factory(s->zone(), _A->get_no_of_virtual_clocks()));
      // add it to the vector of returned results
      return_values init_returned_values;
      init_returned_values.emplace_back(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(tmp));
      // and place it at the return values vector
      ret_val->emplace_back(init_returned_values);
    }
  }

  assert(A_return_values.size() == v_first.size());
  assert(B_return_values.size() == v_second.size());

  for (long unsigned int i = 0; i < v_first.size(); ++i) {

    auto && [status_first, s_first, t_first] = v_first[i];
    tchecker::zg::const_state_sptr_t const_s_first{s_first};

    for(long unsigned int j = 0; j < v_second.size(); ++j) {
      auto && [status_second, s_second, t_second] = v_second[j];
            // the action has to be the same
      if(!t_first->vedge().event_equal(_A->system(), t_second->vedge(), _B->system())) {
        continue;
      }

      tchecker::zg::const_state_sptr_t const_s_second{s_second};
      std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> copy(visited);

      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> inter
        = this->check_for_virt_bisim(const_s_first, t_first, const_s_second, t_second, copy, false);

      auto A_s_ret_val_idx = std::make_tuple<tchecker::intrusive_shared_ptr_t<tchecker::make_shared_t<tchecker::zg::state_t> >*, 
                                             lo_return_values *,
                                             long unsigned int *>(&s_first, &A_return_values, &i);


      auto B_s_ret_val_idx = std::make_tuple<tchecker::intrusive_shared_ptr_t<tchecker::make_shared_t<tchecker::zg::state_t> >*, 
                                             lo_return_values *,
                                             long unsigned int *>(&s_second, &B_return_values, &j);

      auto s_ret_val_idx_together = {&A_s_ret_val_idx, &B_s_ret_val_idx};

      for(auto iter : s_ret_val_idx_together) {
        auto s_cur = *(std::get<0>(*iter));
        auto ret_val_cur = std::get<1>(*iter);
        auto index_cur = std::get<2>(*iter);
        tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> to_add{_A->get_no_of_virtual_clocks() + 1};
        for(auto iter = inter->begin(); iter < inter->end(); iter++) {
          std::shared_ptr<tchecker::zg::zone_t> tmp = tchecker::zg::factory(s_cur->zone().dim());
          if(tchecker::dbm::status_t::NON_EMPTY == (*iter)->logic_and(tmp, s_cur->zone())) {
            to_add.append_zone(tchecker::virtual_constraint::factory(tmp, _A->get_no_of_virtual_clocks()));
          }
        }
        to_add.compress();
       (*ret_val_cur)[*index_cur].emplace_back(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(to_add));
      }
    }
  }

  //std::cout << __FILE__ << ": " << __LINE__ << ": start revert_action_trans" << std::endl;

  for(auto iter : together) {
    auto v_cur = std::get<0>(*iter);
    auto ret_val = std::get<1>(*iter);
    for (long unsigned int i = 0; i < v_cur->size(); ++i) {
      auto && [status_cur, s_cur, t_cur] = (*v_cur)[i];
      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> in_all =
        tchecker::virtual_constraint::contained_in_all( std::ref((*ret_val)[i]), _A->get_no_of_virtual_clocks());

      for(auto iter_in_all = in_all->begin(); iter_in_all < in_all->end(); iter_in_all++) {
        if(!((*iter_in_all)->is_empty())) {
            assert(is_phi_subset_of_a_zone(s_cur->zone().dbm(), s_cur->zone().dim(), (*iter_in_all)->get_no_of_virt_clocks(), **iter_in_all));
            result->append_zone(tchecker::vcg::revert_action_trans((*(std::get<2>(*iter)))->zone(), t_cur->guard_container(), t_cur->reset_container(), t_cur->tgt_invariant_container(), **iter_in_all));
        }
      }
    }

  }

  //std::cout << __FILE__ << ": " << __LINE__ << ": end revert_action_trans" << std::endl;

  assert(
    std::all_of(result->begin(), result->end(),
                [A_state, B_state](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
                {
                  return (is_phi_subset_of_a_zone(A_state->zone().dbm(), A_state->zone().dim(), vc->get_no_of_virt_clocks(), *vc)) ||
                         (is_phi_subset_of_a_zone(B_state->zone().dbm(), B_state->zone().dim(), vc->get_no_of_virt_clocks(), *vc));
                }
    )
  );

  result->compress();

  //std::cout << __FILE__ << ": " << __LINE__ << ": return from check-outgoing-trans" << std::endl;

  return result;

}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

