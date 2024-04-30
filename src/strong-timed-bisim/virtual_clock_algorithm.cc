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
    = this->check_for_virt_bisim(const_first, std::get<2>(sst_first[0]), const_second, std::get<2>(sst_second[0]), empty);

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
                                 std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{

  assert(check_for_virt_bisim_preconditions_check(symb_state_first, symb_trans_first));
  assert(check_for_virt_bisim_preconditions_check(symb_state_second, symb_trans_second));

  _visited_pair_of_states++;

/*
  std::cout << __FILE__ << ": " << __LINE__ << ": _visited_pair_of_states: " << _visited_pair_of_states << std::endl;
  std::cout << __FILE__ << ": " << __LINE__ << ": check-for-virt-bisim" << std::endl;
  std::cout << __FILE__ << ": " << __LINE__ << ": " << symb_state_first->vloc() << std::endl;
  tchecker::dbm::output_matrix(std::cout, symb_state_first->zone().dbm(), symb_state_first->zone().dim());

  std::cout << __FILE__ << ": " << __LINE__ << ": " << symb_state_second->vloc() << std::endl;
  tchecker::dbm::output_matrix(std::cout, symb_state_second->zone().dbm(), symb_state_second->zone().dim());
*/

  // the following is a difference to the original function, done for efficiency reasons.
  // before we do anything, we check whether there even exist an overlap between the symbolic states
  // to do so, we build zone_A \land virtual_constraint(zone_B) and vice versa first and check whether they are empty
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> overhang
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  tchecker::zg::state_sptr_t A_constrained = _A->clone_state(symb_state_first);
  tchecker::zg::state_sptr_t B_constrained = _B->clone_state(symb_state_second);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_A = tchecker::virtual_constraint::factory(symb_state_first->zone(), _A->get_no_of_virtual_clocks());
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_B = tchecker::virtual_constraint::factory(symb_state_second->zone(), _A->get_no_of_virtual_clocks());

  if(
    tchecker::dbm::status_t::EMPTY == phi_B->logic_and(A_constrained->zone(), A_constrained->zone()) ||
    tchecker::dbm::status_t::EMPTY == phi_A->logic_and(B_constrained->zone(), B_constrained->zone())
    )
  {

    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> A_helper = tchecker::virtual_constraint::factory(symb_state_first->zone(), _A->get_no_of_virtual_clocks());
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> B_helper = tchecker::virtual_constraint::factory(symb_state_second->zone(), _A->get_no_of_virtual_clocks());

    overhang->append_zone(A_helper);
    overhang->append_zone(B_helper);
    assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*overhang, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));
    _delete_me++;
    overhang->compress();
    return overhang;
  }


  // we calculate the overhang
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> overhang_A 
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> overhang_B 
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_B->get_no_of_virtual_clocks() + 1);

  phi_B->neg_logic_and(overhang_A, *phi_A);
  phi_A->neg_logic_and(overhang_B, *phi_B);

  overhang->append_container(overhang_A);
  overhang->append_container(overhang_B);

  assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*overhang, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));


  // now that we have calculated the overhang, we can go on with our algorithm
  tchecker::zg::state_sptr_t A_synced = _A->clone_state(A_constrained);
  tchecker::zg::state_sptr_t B_synced = _B->clone_state(B_constrained);

  // we sync the symbolic states
  tchecker::vcg::sync( A_synced->zone_ptr()->dbm(), B_synced->zone_ptr()->dbm(),
                       A_synced->zone_ptr()->dim(), B_synced->zone_ptr()->dim(),
                       _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                       symb_trans_first->reset_container(), symb_trans_second->reset_container());

  assert(tchecker::vcg::are_dbm_synced(A_synced->zone_ptr()->dbm(), B_synced->zone_ptr()->dbm(),
                                       A_synced->zone_ptr()->dim(), B_synced->zone_ptr()->dim(),
                                       _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks()));


  //calculating the future
  tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_synced);
  tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_synced);
  _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), symb_trans_first->tgt_invariant_container());
  _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), symb_trans_second->tgt_invariant_container());

  assert(tchecker::dbm::is_tight(A_epsilon->zone().dbm(), A_epsilon->zone().dim()));
  assert(tchecker::dbm::is_tight(B_epsilon->zone().dbm(), B_epsilon->zone().dim()));


  //calculating the overhang of the future
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> epsilon_overhang
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);


  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_A_epsilon = tchecker::virtual_constraint::factory(A_epsilon->zone(), _A->get_no_of_virtual_clocks());
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_B_epsilon = tchecker::virtual_constraint::factory(B_epsilon->zone(), _A->get_no_of_virtual_clocks());

  overhang_A = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
  overhang_B = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);


  phi_B_epsilon->neg_logic_and(overhang_A, *phi_A_epsilon);
  phi_A_epsilon->neg_logic_and(overhang_B, *phi_B_epsilon);

  epsilon_overhang->append_container(overhang_A);
  epsilon_overhang->append_container(overhang_B);


  // normalizing, to check whether we have already seen this pair.
  tchecker::zg::state_sptr_t A_normed = _A->clone_state(A_epsilon);
  tchecker::zg::state_sptr_t B_normed = _B->clone_state(B_epsilon);

  _A->run_extrapolation(A_normed->zone().dbm(), A_normed->zone().dim(), *(A_normed->vloc_ptr()));
  _B->run_extrapolation(B_normed->zone().dbm(), B_normed->zone().dim(), *(B_normed->vloc_ptr()));

  tchecker::dbm::tighten(A_normed->zone().dbm(), A_normed->zone().dim());
  tchecker::dbm::tighten(B_normed->zone().dbm(), B_normed->zone().dim());

  std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> normalized_pair{A_normed, B_normed};

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> check_trans
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  // only go into the recursion if we have not seen this pair yet
  if(0 == visited.count(normalized_pair)) {
    // add the pair to visited
    visited.insert(normalized_pair);

    // check the outgoing action transitions
    tchecker::zg::const_state_sptr_t const_A_normed{A_normed};
    tchecker::zg::const_state_sptr_t const_B_normed{B_normed};

    check_trans->append_container(check_for_outgoing_transitions(const_A_normed, const_B_normed, visited));
  }


  // collect all results for the epsilon transition
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> epsilon_result
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  epsilon_result->append_container(check_trans);
  epsilon_result->append_container(epsilon_overhang);

  epsilon_result->compress();

  // now revert them
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> epsilon_rev_not_comb
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
  for(auto iter = epsilon_result->begin(); iter < epsilon_result->end(); iter++) {
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> A_helper = tchecker::virtual_constraint::factory(A_epsilon->zone(), _A->get_no_of_virtual_clocks());
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> B_helper = tchecker::virtual_constraint::factory(B_epsilon->zone(), _A->get_no_of_virtual_clocks());
    (*iter)->logic_and(A_helper, *A_helper);
    (*iter)->logic_and(B_helper, *B_helper);

    epsilon_rev_not_comb->append_zone(tchecker::vcg::revert_epsilon_trans(A_synced->zone(), A_epsilon->zone(), *A_helper));
    epsilon_rev_not_comb->append_zone(tchecker::vcg::revert_epsilon_trans(B_synced->zone(), B_epsilon->zone(), *B_helper));
  }

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> epsilon_rev = tchecker::virtual_constraint::combine(*epsilon_rev_not_comb, _A->get_no_of_virtual_clocks());

  epsilon_rev->compress();

  // in the technical report, we rename overhang to sync-reverted. this is not necessary here. Therefore, we proceed to use the variable overhang instead.
  for(auto iter = epsilon_rev->begin(); iter < epsilon_rev->end(); iter++) {
    std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>> sync_reverted
      = tchecker::vcg::revert_sync(A_constrained->zone_ptr()->dbm(), B_constrained->zone_ptr()->dbm(), A_constrained->zone_ptr()->dim(), B_constrained->zone_ptr()->dim(),
                                  _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                                  **iter);

    overhang->append_zone(sync_reverted.first);
    assert(is_phi_subset_of_a_zone(symb_state_first->zone().dbm(), symb_state_first->zone().dim(), _A->get_no_of_virtual_clocks(), *(sync_reverted.first)));

    overhang->append_zone(sync_reverted.second);
    assert(is_phi_subset_of_a_zone(symb_state_second->zone().dbm(), symb_state_second->zone().dim(), _B->get_no_of_virtual_clocks(), *(sync_reverted.second)));
  }

  overhang->compress();

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  result->append_container(tchecker::virtual_constraint::combine(*overhang, _A->get_no_of_virtual_clocks()));

  assert(all_vc_are_sub_vc_of_phi_a_or_phi_b(*result, symb_state_first, symb_state_second, _A->get_no_of_virtual_clocks()));

  _delete_me++;
  //std::cout << __FILE__ << ": " << __LINE__ << ": _delete_me: " << _delete_me << std::endl;

  result->compress();
/*
  std::cout << __FILE__ << ": " << __LINE__ << ": _visited_pair_of_states: " << _visited_pair_of_states << std::endl;
  std::cout << __FILE__ << ": " << __LINE__ << ": check-for-virt-bisim return value" << std::endl;
  std::cout << __FILE__ << ": " << __LINE__ << ": " << symb_state_first->vloc() << symb_state_second->vloc() << std::endl;

  for(auto iter = result->begin(); iter < result->end(); iter++) {
    std::cout << std::endl;
    tchecker::dbm::output_matrix(std::cout, (*iter)->dbm(), (*iter)->dim());
  }
*/
  return result;
}


std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions( tchecker::zg::const_state_sptr_t A_state,
                                            tchecker::zg::const_state_sptr_t B_state,
                                            std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{
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
        = this->check_for_virt_bisim(const_s_first, t_first, const_s_second, t_second, copy);

      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_append_A 
        = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_append_B 
        = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

      for(auto iter = inter->begin(); iter < inter->end(); iter++) {
        std::shared_ptr<tchecker::zg::zone_t> tmp_first = tchecker::zg::factory(s_first->zone().dim());
        std::shared_ptr<tchecker::zg::zone_t> tmp_second = tchecker::zg::factory(s_second->zone().dim());

        
        if(tchecker::dbm::NON_EMPTY == (*iter)->logic_and(tmp_first, s_first->zone()) ) {
          to_append_A->append_zone(tchecker::virtual_constraint::factory(tmp_first, _A->get_no_of_virtual_clocks()));
        }

        if(tchecker::dbm::NON_EMPTY == (*iter)->logic_and(tmp_second, s_second->zone()) ) {
          to_append_B->append_zone(tchecker::virtual_constraint::factory(tmp_second, _A->get_no_of_virtual_clocks()));
        }
      }

      to_append_A->compress();
      to_append_B->compress();

      A_return_values[i].emplace_back(to_append_A);
      B_return_values[j].emplace_back(to_append_B);

    }
  }

  //std::cout << __FILE__ << ": " << __LINE__ << ": start revert_action_trans" << std::endl;

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> collapsed
     = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

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
            collapsed->append_zone(tchecker::vcg::revert_action_trans((*(std::get<2>(*iter)))->zone(), t_cur->guard_container(), t_cur->reset_container(), t_cur->tgt_invariant_container(), **iter_in_all));
        }
      }
    }

  }

  //std::cout << __FILE__ << ": " << __LINE__ << ": end revert_action_trans" << std::endl;

  assert(
    std::all_of(collapsed->begin(), collapsed->end(),
                [A_state, B_state](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
                {
                  return (is_phi_subset_of_a_zone(A_state->zone().dbm(), A_state->zone().dim(), vc->get_no_of_virt_clocks(), *vc)) ||
                         (is_phi_subset_of_a_zone(B_state->zone().dbm(), B_state->zone().dim(), vc->get_no_of_virt_clocks(), *vc));
                }
    )
  );

  collapsed->compress();

  //std::cout << __FILE__ << ": " << __LINE__ << ": return from check-outgoing-trans" << std::endl;

  return combine(*collapsed, collapsed->dim() - 1);
}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

