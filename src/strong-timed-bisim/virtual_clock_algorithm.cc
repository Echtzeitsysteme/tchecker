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

  assert(tchecker::dbm::is_consistent(symb_state->zone().dbm(), symb_state->zone().dim()));
  assert(tchecker::dbm::is_tight(symb_state->zone().dbm(), symb_state->zone().dim()));

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

  // if the states are not synced, the last transition must have been an action transition
  if(!tchecker::vcg::are_zones_synced(A_state->zone(), B_state->zone(), _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks())) {
    return true;
  }

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


  //std::cout << __FILE__ << ": " << __LINE__ << ": _visited_pair_of_states: " << _visited_pair_of_states << std::endl;
  //std::cout << __FILE__ << ": " << __LINE__ << ": check-for-virt-bisim" << std::endl;
  //std::cout << __FILE__ << ": " << __LINE__ << ": " << A_state->vloc() << std::endl;
  //tchecker::dbm::output_matrix(std::cout, A_state->zone().dbm(), A_state->zone().dim());

  //std::cout << __FILE__ << ": " << __LINE__ << ": " << B_state->vloc() << std::endl;
  //tchecker::dbm::output_matrix(std::cout, B_state->zone().dbm(), B_state->zone().dim());

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


  if(do_an_epsilon_transition(A_cloned, A_trans, B_cloned, B_trans))  {

    tchecker::vcg::sync( A_cloned->zone(), B_cloned->zone(),
                         _A->get_no_of_original_clocks(), _B->get_no_of_original_clocks(),
                         A_trans->reset_container(), B_trans->reset_container());

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
    // normalizing, to check whether we have already seen this pair.
    _A->run_extrapolation(A_cloned->zone().dbm(), A_cloned->zone().dim(), *(A_cloned->vloc_ptr()));
    _B->run_extrapolation(B_cloned->zone().dbm(), B_cloned->zone().dim(), *(B_cloned->vloc_ptr()));

    tchecker::dbm::tighten(A_cloned->zone().dbm(), A_cloned->zone().dim());
    tchecker::dbm::tighten(B_cloned->zone().dbm(), B_cloned->zone().dim());

    std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> normalized_pair{A_cloned, B_cloned};

    if(0 != visited.count(normalized_pair)) {
      return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);
    }

    visited.insert(normalized_pair);

    // we go on with the non-normalized symbolic states
    A_cloned = _A->clone_state(A_state);
    B_cloned = _B->clone_state(B_state);

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
        return tchecker::virtual_constraint::combine(*contradiction, _A->get_no_of_virtual_clocks());
      }

    }

    return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  }
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions_single_empty(tchecker::zg::zone_t const & zone, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans) {

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
    = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  for(auto cur = trans.begin(); cur < trans.end(); cur++) {
    auto && [status, s, t] = **cur;
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi = tchecker::virtual_constraint::factory(s->zone(), _A->get_no_of_virtual_clocks());
    result->append_zone(tchecker::vcg::revert_action_trans(zone, t->guard_container(), t->reset_container(), t->tgt_invariant_container(), *phi));
  }

  return result;
}

std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>>
Lieb_et_al::extract_zone_without_contradictions(tchecker::zg::zone_t const & zone, std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions)
{

  std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> result =
    std::make_shared<tchecker::zone_container_t<tchecker::zg::zone_t>>(zone.dim());

  if(contradictions->is_empty()) {
    result->append_zone(zone);
    return result;
  }


  auto vc_result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);
  vc_result->append_zone(tchecker::virtual_constraint::factory(zone, _A->get_no_of_virtual_clocks()));

  // zone && not phi_1 && not phi_2 && not phi_3 && ... 
  for(auto cur = contradictions->begin(); cur < contradictions->end(); cur++) {
    auto inter_result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks()+1);
    for(auto vc = vc_result->begin(); vc < vc_result->end(); vc++) {
      auto helper = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);
      (*cur)->neg_logic_and(helper, **vc);
      inter_result->append_container(helper);
      inter_result->compress();
    }
    vc_result = inter_result;
  }

  for(auto cur : *vc_result) {
    std::shared_ptr<tchecker::zg::zone_t> helper = tchecker::zg::factory(zone.dim());
    cur->logic_and(helper, zone);
    result->append_zone(helper);
  }

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
Lieb_et_al::check_splitted_transitions(std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> zones_A, tchecker::zg::transition_sptr_t trans_A, tchecker::zg::state_sptr_t state_A,
                                       std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> zones_B, tchecker::zg::transition_sptr_t trans_B, tchecker::zg::state_sptr_t state_B,
                                       std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{
  assert(check_disjointness(zones_A));
  assert(check_disjointness(zones_B));

  tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> cut_offs{zones_A->size(), zones_B->size(), _A->get_no_of_virtual_clocks() + 1};

  std::vector<std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>> to_check;

  for(size_t idx_A = 0; idx_A < zones_A->size(); idx_A++) {
    std::shared_ptr<tchecker::zg::zone_t> zone_A = (*zones_A)[idx_A];
    for(size_t idx_B = 0; idx_B < zones_B->size(); idx_B++) {
      std::shared_ptr<tchecker::zg::zone_t> zone_B = (*zones_B)[idx_B];

      (cut_offs.get(idx_A, idx_B))->append_container(zone_A->get_virtual_overhang_in_both_directions(*zone_B, _A->get_no_of_virtual_clocks()));

      std::shared_ptr<tchecker::zg::zone_t> zone_A_copy = factory(*zone_A);
      std::shared_ptr<tchecker::zg::zone_t> zone_B_copy = factory(*zone_B);

      auto phi_A = tchecker::virtual_constraint::factory(*zone_A, _A->get_no_of_virtual_clocks());
      auto phi_B = tchecker::virtual_constraint::factory(*zone_B, _A->get_no_of_virtual_clocks());

      if(tchecker::dbm::status_t::EMPTY != phi_B->logic_and(zone_A_copy, *zone_A_copy) && 
         tchecker::dbm::status_t::EMPTY != phi_A->logic_and(zone_B_copy, *zone_B_copy)) {
        to_check.emplace_back(std::pair(zone_A_copy, zone_B_copy));
      }

    }
  }

  auto contradiction = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

  for(size_t idx_A = 0; idx_A < zones_A->size(); idx_A++) {
    auto lo_sets = cut_offs.get_line(idx_A);
    auto phi_idx_A = tchecker::virtual_constraint::factory((*zones_A)[idx_A], _A->get_no_of_virtual_clocks());
    auto container_with_phi_idx_A = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*phi_idx_A);

    lo_sets->emplace_back(container_with_phi_idx_A);
    contradiction->append_container(tchecker::virtual_constraint::contained_in_all(*lo_sets, _A->get_no_of_virtual_clocks()));
  }

  for(size_t idx_B = 0; idx_B < zones_B->size(); idx_B++) {
    auto lo_sets = cut_offs.get_column(idx_B);
    auto phi_idx_B = tchecker::virtual_constraint::factory((*zones_B)[idx_B], _A->get_no_of_virtual_clocks());
    auto container_with_phi_idx_B = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*phi_idx_B);

    lo_sets->emplace_back(container_with_phi_idx_B);
    contradiction->append_container(tchecker::virtual_constraint::contained_in_all(*lo_sets, _A->get_no_of_virtual_clocks()));
  }

  if(!(contradiction->is_empty())) {
    return contradiction;
  }

  for(auto cur = to_check.begin(); cur < to_check.end(); cur++) {

    tchecker::zg::state_sptr_t clone_A = _A->clone_state(state_A);
    tchecker::zg::state_sptr_t clone_B = _B->clone_state(state_B);

    clone_A->replace_zone(*std::get<0>(*cur));
    clone_B->replace_zone(*std::get<1>(*cur));

    tchecker::zg::const_state_sptr_t const_A{clone_A};
    tchecker::zg::const_state_sptr_t const_B{clone_B};

    std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> copy(visited);
    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> cont
      = this->check_for_virt_bisim(const_A, trans_A, const_B, trans_B, copy);

    cont->compress();
    cont = tchecker::virtual_constraint::combine(*cont, _A->get_no_of_virtual_clocks());
    cont->compress();

    // since the subzones are disjoint and any pair is virtual equivalent, any found contradiction is an overall contradiction.
    if(!(cont->is_empty())) {
      return cont;
    }
  }

  return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions( tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                            std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_A, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_B,
                                            std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited)
{

  if(0 == trans_A.size() || 0 == trans_B.size()) {
    return check_for_outgoing_transitions_single_empty( ( 0 == trans_B.size() ? zone_A : zone_B), ( 0 == trans_B.size() ? trans_A : trans_B));
  }

  tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> return_values{trans_A.size(), trans_B.size(), _A->get_no_of_virtual_clocks() + 1};
  std::vector<bool> finished(trans_A.size() * trans_B.size(), false);

  do {
    for(size_t idx_A = 0; idx_A < trans_A.size(); idx_A++) {
      auto && [status_A, s_A, t_A] = *(trans_A[idx_A]);
      for(size_t idx_B = 0; idx_B < trans_B.size(); idx_B++) {
        if(finished[idx_A *trans_A.size() + idx_B]) {
          continue;
        }
        auto && [status_B, s_B, t_B] = *(trans_B[idx_B]);

        std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> found = return_values.get(idx_A, idx_B);

        std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> cont_free_zones_A = extract_zone_without_contradictions(s_A->zone(), found);
        std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> cont_free_zones_B = extract_zone_without_contradictions(s_B->zone(), found);;

        cont_free_zones_A->compress();
        cont_free_zones_B->compress();

        if(cont_free_zones_A->is_empty() || cont_free_zones_B->is_empty()) {
          finished[idx_A * trans_B.size() + idx_B] = true;
          continue;
        }

        std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> new_cont
          = check_splitted_transitions(cont_free_zones_A, t_A, s_A, cont_free_zones_B, t_B, s_B, visited);

        if(new_cont->is_empty()) {
          finished[idx_A *trans_A.size() + idx_B] = true;
        } else {
          found->append_container(new_cont);
          found->compress();
        }

        std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradiction = 
          find_contradiction(zone_A, zone_B, trans_A, trans_B, return_values);

        if(!(contradiction->is_empty())) {
          return contradiction;
        }

      }
    }
  } while(std::count(finished.begin(), finished.end(), false) > 0);

  return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_A->get_no_of_virtual_clocks() + 1);

}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

