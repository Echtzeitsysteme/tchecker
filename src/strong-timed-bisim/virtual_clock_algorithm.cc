/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"
#include "tchecker/strong-timed-bisim/contradiction_searcher.hh"
#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/vcg/revert_transitions.hh"
#include "tchecker/vcg/sync.hh"
#include "tchecker/dbm/dbm.hh"

namespace tchecker {

namespace strong_timed_bisim {

Lieb_et_al::Lieb_et_al(std::shared_ptr<tchecker::vcg::vcg_t> input_first, std::shared_ptr<tchecker::vcg::vcg_t> input_second)
  : _A(input_first), _B(input_second), _visited_pair_of_states(0)
{
  assert(_A->get_no_of_virtual_clocks() == _B->get_no_of_virtual_clocks());
  assert(_A->get_urgent_or_committed() == _B->get_urgent_or_committed());
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

  assert(tchecker::dbm::is_tight(symb_state->zone().dbm(), symb_state->zone().dim()));
  assert(tchecker::dbm::is_consistent(symb_state->zone().dbm(), symb_state->zone().dim()));


  assert(!tchecker::dbm::is_empty_0(symb_state->zone().dbm(), symb_state->zone().dim()));

  // we check whether the resets of the zones are matching the resets of the transitions. WARNING: This works for reset to zero only!
  for(auto iter = symb_trans->reset_container().begin(); iter < symb_trans->reset_container().end(); iter++) {

    if(!iter->reset_to_zero()) {
      std::cerr << __FILE__ << ": " << __LINE__ << ": the reset " << *iter << " is not a reset to zero" << std::endl;
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

bool Lieb_et_al::do_an_epsilon_transition(tchecker::zg::state_sptr_t A_state, tchecker::zg::transition_sptr_t A_trans,
                                          tchecker::zg::state_sptr_t B_state, tchecker::zg::transition_sptr_t B_trans) {

  assert(tchecker::vcg::are_zones_synced(A_state->zone(), B_state->zone(), _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks()));

  tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_state);
  tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_state);

  if(tchecker::ta::delay_allowed(_A->system(), A_state->vloc())) {
    _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), A_trans->tgt_invariant_container());
  }

  if(tchecker::ta::delay_allowed(_B->system(), B_state->vloc())) {
    _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), B_trans->tgt_invariant_container());
  }

  return (A_state->zone() != A_epsilon->zone() || B_state->zone() != B_epsilon->zone());

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_virt_bisim(tchecker::zg::const_state_sptr_t A_state, tchecker::zg::transition_sptr_t A_trans,
                                 tchecker::zg::const_state_sptr_t B_state, tchecker::zg::transition_sptr_t B_trans,
                                 std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{

  assert(check_for_virt_bisim_preconditions_check(A_state, A_trans));
  assert(check_for_virt_bisim_preconditions_check(B_state, B_trans));


  _visited_pair_of_states++;


//  std::cout << __FILE__ << ": " << __LINE__ << ": _visited_pair_of_states: " << _visited_pair_of_states << std::endl;
//  std::cout << __FILE__ << ": " << __LINE__ << ": check-for-virt-bisim" << std::endl;
//  std::cout << __FILE__ << ": " << __LINE__ << ": " << A_state->vloc() << std::endl;
//  tchecker::dbm::output_matrix(std::cout, A_state->zone().dbm(), A_state->zone().dim());

//  std::cout << __FILE__ << ": " << __LINE__ << ": " << B_state->vloc() << std::endl;
//  tchecker::dbm::output_matrix(std::cout, B_state->zone().dbm(), B_state->zone().dim());

  // check for virtual equivalence of A_state->zone() and B_state->zone()
  if(!A_state->zone().is_virtual_equivalent(B_state->zone(), _A->get_no_of_virtual_clocks()))
  {
    return A_state->zone().get_virtual_overhang_in_both_directions(B_state->zone(), _A->get_no_of_virtual_clocks());
  }

  tchecker::zg::state_sptr_t A_cloned = _A->clone_state(A_state);
  tchecker::zg::state_sptr_t B_cloned = _B->clone_state(B_state);

  // if there is an urgent or committed location, there is an extra virtual clock that must be reset
  if(_A->get_urgent_or_committed() && (!tchecker::ta::delay_allowed(_A->system(), A_state->vloc()) || !tchecker::ta::delay_allowed(_B->system(), B_state->vloc()))) {
    reset_to_value(A_cloned->zone().dbm(), A_cloned->zone_ptr()->dim(), _A->get_no_of_original_clocks() + _A->get_no_of_virtual_clocks(), 0);
    reset_to_value(B_cloned->zone().dbm(), B_cloned->zone_ptr()->dim(), _B->get_no_of_original_clocks() + _B->get_no_of_virtual_clocks(), 0);
  }

  tchecker::vcg::sync( A_cloned->zone(), B_cloned->zone(),
                       _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                       A_trans->reset_container(), B_trans->reset_container());


  if(do_an_epsilon_transition(A_cloned, A_trans, B_cloned, B_trans))  {
    tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_cloned);
    tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_cloned);

    if(tchecker::ta::delay_allowed(_A->system(), A_state->vloc())) {
      _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), A_trans->tgt_invariant_container());
    }

    if(tchecker::ta::delay_allowed(_B->system(), B_state->vloc())) {
      _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), B_trans->tgt_invariant_container());
    }

    tchecker::zg::const_state_sptr_t A_epsilon_const{A_epsilon};
    tchecker::zg::const_state_sptr_t B_epsilon_const{B_epsilon};

    _visited_pair_of_states--; // we don't count the epsilon transition

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> eps_result
      = check_for_virt_bisim(A_epsilon_const, A_trans, B_epsilon_const, B_trans, visited);

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradiction
      = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);

    for(auto cur = eps_result->begin(); cur < eps_result->end(); cur++) {
      contradiction->append_zone(tchecker::vcg::revert_epsilon_trans(A_cloned->zone(), A_epsilon->zone(), **cur));
      contradiction->append_zone(tchecker::vcg::revert_epsilon_trans(B_cloned->zone(), B_epsilon->zone(), **cur));
    }

    contradiction->compress();

    A_cloned = _A->clone_state(A_state);
    B_cloned = _B->clone_state(B_state);

    if(_A->get_urgent_or_committed() && (!tchecker::ta::delay_allowed(_A->system(), A_state->vloc()) || !tchecker::ta::delay_allowed(_B->system(), B_state->vloc()))) {
      reset_to_value(A_cloned->zone().dbm(), A_cloned->zone_ptr()->dim(), _A->get_no_of_original_clocks() + _A->get_no_of_virtual_clocks(), 0);
      reset_to_value(B_cloned->zone().dbm(), B_cloned->zone_ptr()->dim(), _B->get_no_of_original_clocks() + _B->get_no_of_virtual_clocks(), 0);
    }

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> sync_reverted
      = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

    for(auto phi = contradiction->begin(); phi < contradiction->end(); phi++) {
      auto pair = tchecker::vcg::revert_sync(A_cloned->zone(), B_cloned->zone(), _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(), **phi);
      sync_reverted->append_zone(pair.first);
      sync_reverted->append_zone(pair.second);
    }

    sync_reverted->compress();
    sync_reverted = tchecker::virtual_constraint::combine(*sync_reverted, _A->get_no_of_virtual_clocks());
    sync_reverted->compress();
    return sync_reverted;

  } else {

    auto A_norm = _A->clone_state(A_cloned);
    auto B_norm = _B->clone_state(B_cloned);

    // normalizing, to check whether we have already seen this pair.
    _A->run_extrapolation(A_norm->zone().dbm(), A_norm->zone().dim(), *(A_norm->vloc_ptr()));
    _B->run_extrapolation(B_norm->zone().dbm(), B_norm->zone().dim(), *(B_norm->vloc_ptr()));

    tchecker::dbm::tighten(A_norm->zone().dbm(), A_norm->zone().dim());
    tchecker::dbm::tighten(B_norm->zone().dbm(), B_norm->zone().dim());

    std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> normalized_pair{A_norm, B_norm};

    if(0 != visited.count(normalized_pair)) {
      return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);
    }

    visited.insert(normalized_pair);

    // we go on with the non-normalized symbolic states
    assert(tchecker::dbm::is_tight(A_cloned->zone().dbm(), A_cloned->zone().dim()));
    assert(tchecker::dbm::is_tight(B_cloned->zone().dbm(), B_cloned->zone().dim()));

    std::vector<tchecker::vcg::vcg_t::sst_t> v_A, v_B;

    tchecker::zg::const_state_sptr_t A_cloned_const{A_cloned};
    tchecker::zg::const_state_sptr_t B_cloned_const{B_cloned};

    _A->next(A_cloned_const, v_A);
    _B->next(B_cloned_const, v_B);

    std::set<std::set<std::string>> avail_events;

    auto add_to_avail_events = [](std::set<std::set<std::string>> & avail_events, std::vector<tchecker::vcg::vcg_t::sst_t> & v, const tchecker::ta::system_t & system)
                                {
                                  for(auto&& [status, s, t] : v) {
                                    avail_events.insert(t->vedge().event_names(system));
                                  }
                                };

    add_to_avail_events(avail_events, v_A, _A->system());
    add_to_avail_events(avail_events, v_B, _B->system());


    for(auto& symbol : avail_events) {
      auto add_to_transition_list 
        = [](std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_list, std::vector<tchecker::vcg::vcg_t::sst_t> & v, const tchecker::ta::system_t & system, std::set<std::string> symbol)
             {
               for(auto& cur_trans : v) {
                 assert(tchecker::dbm::is_tight(std::get<1>(cur_trans)->zone().dbm(), std::get<1>(cur_trans)->zone().dim()));
                 if(std::get<2>(cur_trans)->vedge().event_equal(system, symbol)) {
                   trans_list.emplace_back(&cur_trans);
                 }
               }
             };

      std::vector<tchecker::vcg::vcg_t::sst_t *> trans_A;
      std::vector<tchecker::vcg::vcg_t::sst_t *> trans_B;

      add_to_transition_list(trans_A, v_A, _A->system(), symbol);
      add_to_transition_list(trans_B, v_B, _B->system(), symbol);

      std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradiction
        = check_for_outgoing_transitions( A_cloned->zone(), B_cloned->zone(), trans_A, trans_B, visited);

      if(!(contradiction->is_empty())) {
        std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> sync_reverted
          = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

        for(auto phi = contradiction->begin(); phi < contradiction->end(); phi++) {
          auto pair = tchecker::vcg::revert_sync(A_cloned->zone(), B_cloned->zone(), _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(), **phi);
          sync_reverted->append_zone(pair.first);
          sync_reverted->append_zone(pair.second);
        }

        sync_reverted->compress();
        sync_reverted = tchecker::virtual_constraint::combine(*sync_reverted, _A->get_no_of_virtual_clocks());
        sync_reverted->compress();
        return sync_reverted;
      }
    }

    return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  }
}


std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::extract_vc_without_contradictions(tchecker::zg::zone_t const & zone, std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions)
{

  assert(tchecker::dbm::is_tight(zone.dbm(), zone.dim()));
  assert(tchecker::dbm::is_consistent(zone.dbm(), zone.dim()));

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result =
    std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);

  result->append_zone(tchecker::virtual_constraint::factory(zone, _A->get_no_of_virtual_clocks()));

  // zone && not phi_1 && not phi_2 && not phi_3 && ... 
  for(auto cur = contradictions->begin(); cur < contradictions->end(); cur++) {
    auto inter_result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);
    for(auto vc = result->begin(); vc < result->end(); vc++) {
      auto helper = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
      (*cur)->neg_logic_and(helper, **vc);
      inter_result->append_container(helper);
      inter_result->compress();
    }
    result = inter_result;
  }

  result->compress();
  result = tchecker::virtual_constraint::combine(*result, _A->get_no_of_virtual_clocks());
  result->compress();
  return result;

}

bool check_disjointness(std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> zones)
{
  for(auto i = zones->begin(); i < zones->end(); i++) {
    for(auto j = i+1; j < zones->end(); j++) {
      if(! tchecker::dbm::disjoint((*i)->dbm(), (*j)->dbm(), (*i)->dim())) {
        return false;
      }
    }
  }

  return true;

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_target_pair(tchecker::zg::state_sptr_t target_state_A, tchecker::zg::transition_sptr_t trans_A,
                              tchecker::zg::state_sptr_t target_state_B, tchecker::zg::transition_sptr_t trans_B,
                              std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> already_found_contradictions,
                              std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{

  assert(tchecker::dbm::is_tight(target_state_A->zone().dbm(), target_state_A->zone().dim()));
  assert(tchecker::dbm::is_tight(target_state_B->zone().dbm(), target_state_B->zone().dim()));
  assert(target_state_A->zone().is_virtual_equivalent(target_state_B->zone(), _A->get_no_of_virtual_clocks()));

  std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> copy(visited);

  tchecker::zg::state_sptr_t clone_A = _A->clone_state(target_state_A);
  tchecker::zg::state_sptr_t clone_B = _B->clone_state(target_state_B);

  auto without_cont = extract_vc_without_contradictions(target_state_A->zone(), already_found_contradictions);

  for(auto cur : *without_cont) {
    tchecker::zg::state_sptr_t clone_A = _A->clone_state(target_state_A);
    cur->logic_and(clone_A->zone(), target_state_A->zone());
    tchecker::zg::const_state_sptr_t final_A {clone_A};

    tchecker::zg::state_sptr_t clone_B = _B->clone_state(target_state_B);
    cur->logic_and(clone_B->zone(), target_state_B->zone());
    tchecker::zg::const_state_sptr_t final_B{clone_B};

    assert(tchecker::dbm::is_tight(final_A->zone().dbm(), final_A->zone().dim()));
    assert(tchecker::dbm::is_tight(final_B->zone().dbm(), final_B->zone().dim()));

    auto new_cont = this->check_for_virt_bisim(final_A, trans_A, final_B, trans_B, copy);

    new_cont->compress();

    if(! new_cont->is_empty()) {
      auto result = tchecker::virtual_constraint::combine(*new_cont, _A->get_no_of_virtual_clocks());
      result->compress();
      return new_cont;
    }
  }

  return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions( tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                            std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_A, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_B,
                                            std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{

  assert(tchecker::dbm::is_tight(zone_A.dbm(), zone_A.dim()));
  assert(tchecker::dbm::is_tight(zone_B.dbm(), zone_B.dim()));

  if(0 == trans_A.size() && 0 == trans_B.size()) {
    return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
  }

  if(0 == trans_A.size() || 0 == trans_B.size()) {
    auto result =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

    for(auto cur : trans_A) {
      auto && [status_A, s_A, t_A] = *cur;
      auto target = tchecker::virtual_constraint::factory(s_A->zone(), _A->get_no_of_virtual_clocks());
      auto to_append = tchecker::vcg::revert_action_trans(zone_A, t_A->guard_container(), t_A->reset_container(), t_A->tgt_invariant_container(), *target);      
      result->append_zone(to_append);
    }

    for(auto cur : trans_B) {
      auto && [status_B, s_B, t_B] = *cur;
      auto target = tchecker::virtual_constraint::factory(s_B->zone(), _B->get_no_of_virtual_clocks());
      auto to_append = tchecker::vcg::revert_action_trans(zone_B, t_B->guard_container(), t_B->reset_container(), t_B->tgt_invariant_container(), *target); 
      result->append_zone(to_append);
    }

    result->compress();
    result = tchecker::virtual_constraint::combine(*result, _A->get_no_of_virtual_clocks());
    result->compress();

    return result;

  }

  tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> found_cont{trans_A.size(), trans_B.size(), _A->get_no_of_virtual_clocks() + 1};
  std::vector<std::vector<bool>> finished(trans_A.size(), std::vector<bool>(trans_B.size(), false)); // init the finished matrix with false

  contradiction_searcher_t con_searcher{trans_A, trans_B, _A->get_no_of_virtual_clocks()};

  // we add an optimization here. We first check if the union of the targets of both sides are virtually equivalent. If they are not, we can already stop here.
  // We do this by running the search_contradiction function without any found contradiction and checking, whether this already returns a contradiction.

  auto contradiction = con_searcher.search_contradiction(zone_A, zone_B, trans_A, trans_B, found_cont);

  if(!(contradiction->is_empty())) {
    return contradiction;
  }


  do {
    for(size_t idx_A = 0; idx_A < trans_A.size(); idx_A++) {
      auto && [status_A, s_A, t_A] = *(trans_A[idx_A]);
      assert(tchecker::dbm::is_tight(s_A->zone().dbm(), s_A->zone().dim()));

      for(size_t idx_B = 0; idx_B < trans_B.size(); idx_B++) {
        if(finished[idx_A][idx_B]) {
          continue;
        }
        auto && [status_B, s_B, t_B] = *(trans_B[idx_B]);
        assert(tchecker::dbm::is_tight(s_B->zone().dbm(), s_B->zone().dim()));

        tchecker::zg::state_sptr_t s_A_constrained = _A->clone_state(s_A);
        auto s_A_vc = tchecker::virtual_constraint::factory(s_A->zone(), _A->get_no_of_virtual_clocks());
        tchecker::zg::state_sptr_t s_B_constrained = _B->clone_state(s_B);
        auto s_B_vc = tchecker::virtual_constraint::factory(s_B->zone(), _B->get_no_of_virtual_clocks());

        s_B_vc->logic_and(s_A_constrained->zone(), s_A_constrained->zone());
        s_A_vc->logic_and(s_B_constrained->zone(), s_B_constrained->zone());

        if(s_A_constrained->zone().is_empty() || s_B_constrained->zone().is_empty()) {
          continue;
        }

        auto new_cont = check_target_pair(s_A_constrained, t_A, s_B_constrained, t_B, found_cont.get(idx_A, idx_B), visited);

        if(new_cont->is_empty()) {
          finished[idx_A][idx_B] = true;
        } else {
          found_cont.get(idx_A, idx_B)->append_container(new_cont);
          found_cont.get(idx_A, idx_B)->compress();
        }
      }
    }

    auto contradiction = con_searcher.search_contradiction(zone_A, zone_B, trans_A, trans_B, found_cont);

    if(!(contradiction->is_empty())) {
      return contradiction;
    }

  } while(con_searcher.contradiction_still_possible(zone_A, zone_B, trans_A, trans_B, found_cont, finished));

  return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

