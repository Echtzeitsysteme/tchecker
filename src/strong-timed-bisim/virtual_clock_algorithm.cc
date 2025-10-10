/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/strong-timed-bisim/contradiction_searcher.hh"
#include "tchecker/vcg/revert_transitions.hh"
#include "tchecker/vcg/sync.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

Lieb_et_al::Lieb_et_al(std::shared_ptr<tchecker::vcg::vcg_t> input_first, std::shared_ptr<tchecker::vcg::vcg_t> input_second,
                       bool generate_witness)
    : _A(input_first), _B(input_second), _visited_pair_of_states(0), _non_bisim_cache(input_first->get_no_of_virtual_clocks()),
      _witness(generate_witness)
{
  assert(_A->get_no_of_virtual_clocks() == _B->get_no_of_virtual_clocks());
  assert(_A->get_urgent_or_committed() == _B->get_urgent_or_committed());
}

tchecker::strong_timed_bisim::stats_t Lieb_et_al::run()
{

  tchecker::strong_timed_bisim::stats_t stats;

  stats.set_start_time();

  std::vector<tchecker::zg::zg_t::sst_t> sst_first;
  std::vector<tchecker::zg::zg_t::sst_t> sst_second;

  this->_A->initial(sst_first);
  this->_B->initial(sst_second);

  if (STATE_OK != std::get<0>(sst_first[0]) || STATE_OK != std::get<0>(sst_second[0]))
    throw std::runtime_error("problems with initial state");

  tchecker::zg::const_state_sptr_t const_first{std::get<1>(sst_first[0])};
  tchecker::zg::const_state_sptr_t const_second{std::get<1>(sst_second[0])};

  visited_map_t visited(_A->get_no_of_virtual_clocks(), _A, _B);

  auto result =
      this->check_for_virt_bisim(const_first, std::get<2>(sst_first[0]), const_second, std::get<2>(sst_second[0]), visited);
  
  if (_witness && result->contradiction_free()) {
    stats.init_witness(_A, _B);
    stats.witness()->create_witness_from_visited(visited, std::get<1>(sst_first[0]), std::get<1>(sst_second[0]));
  } else if (_witness) {
    tchecker::clockbounds::bound_t max_delay = std::max(_A->extrapolation_max(), _B->extrapolation_max());
    stats.init_counterexample(_A, _B, std::get<1>(sst_first[0]), std::get<1>(sst_second[0]), clock_rational_value_t(max_delay, 1));
    stats.counterexample()->create_cont_from_non_bisim_cache(
                               _non_bisim_cache, 
                               std::make_shared<tchecker::clock_constraint_container_t>(std::get<2>(sst_first[0])->tgt_invariant_container()), 
                               std::make_shared<tchecker::clock_constraint_container_t>(std::get<2>(sst_second[0])->tgt_invariant_container()));
  }

  stats.set_end_time();

  stats.set_visited_pair_of_states(_visited_pair_of_states);
  stats.set_relationship_fulfilled(result->contradiction_free());

  return stats;
}

void check_for_virt_bisim_preconditions_check(tchecker::zg::const_state_sptr_t symb_state,
                                              tchecker::zg::transition_sptr_t symb_trans)
{

  assert(tchecker::dbm::is_tight(symb_state->zone().dbm(), symb_state->zone().dim()));
  assert(tchecker::dbm::is_consistent(symb_state->zone().dbm(), symb_state->zone().dim()));

  assert(!tchecker::dbm::is_empty_0(symb_state->zone().dbm(), symb_state->zone().dim()));

  // we check whether the resets of the zones are matching the resets of the transitions. WARNING: This works for reset to zero
  // only!
  for (auto iter = symb_trans->reset_container().begin(); iter < symb_trans->reset_container().end(); iter++) {

    if (!iter->reset_to_zero()) {
      std::cerr << __FILE__ << ": " << __LINE__ << ": the reset " << *iter << " is not a reset to zero" << std::endl;
      throw std::runtime_error(std::string("Only clock resets to zero are allowed"));
    }
  }
}

std::shared_ptr<algorithm_return_value_t> Lieb_et_al::check_for_virt_bisim(tchecker::zg::const_state_sptr_t A_state,
                                                                           tchecker::zg::transition_sptr_t A_trans,
                                                                           tchecker::zg::const_state_sptr_t B_state,
                                                                           tchecker::zg::transition_sptr_t B_trans,
                                                                           visited_map_t & visited)
{

  check_for_virt_bisim_preconditions_check(A_state, A_trans);
  check_for_virt_bisim_preconditions_check(B_state, B_trans);

  _visited_pair_of_states++;

  // std::cout << __FILE__ << ": " << __LINE__ << ": _visited_pair_of_states: " << _visited_pair_of_states << std::endl;
  // std::cout << __FILE__ << ": " << __LINE__ << ": check-for-virt-bisim" << std::endl;

  // std::cout << __FILE__ << ": " << __LINE__ << ": " << tchecker::to_string(A_state->vloc(), _A->system()) << std::endl;
  // tchecker::dbm::output_matrix(std::cout, A_state->zone().dbm(), A_state->zone().dim());

  // std::cout << __FILE__ << ": " << __LINE__ << ": " << tchecker::to_string(B_state->vloc(), _B->system()) << std::endl;
  // tchecker::dbm::output_matrix(std::cout, B_state->zone().dbm(), B_state->zone().dim());

  // check for virtual equivalence of A_state->zone() and B_state->zone()
  if (!A_state->zone().is_virtual_equivalent(B_state->zone(), _A->get_no_of_virtual_clocks())) {
    auto result = std::make_shared<algorithm_return_value_t>(_A->get_no_of_virtual_clocks(), A_state, B_state);
    result->add_to_contradictions(
        A_state->zone().get_virtual_overhang_in_both_directions(B_state->zone(), _A->get_no_of_virtual_clocks()));
    return result;
  }

  tchecker::zg::state_sptr_t A_synced = _A->clone_state(A_state);
  tchecker::zg::state_sptr_t B_synced = _B->clone_state(B_state);
  tchecker::vcg::sync_vc_t syncer{_A, _B};
  syncer.sync_with_urgent(A_synced, B_synced, A_trans->reset_container(), B_trans->reset_container());

  if (do_an_epsilon_transition(A_synced, A_trans, B_synced, B_trans)) {
    tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_synced);
    tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_synced);

    if (tchecker::ta::delay_allowed(_A->system(), A_state->vloc())) {
      _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), A_trans->tgt_invariant_container());
    }

    if (tchecker::ta::delay_allowed(_B->system(), B_state->vloc())) {
      _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), B_trans->tgt_invariant_container());
    }

    tchecker::zg::const_state_sptr_t A_epsilon_const{A_epsilon};
    tchecker::zg::const_state_sptr_t B_epsilon_const{B_epsilon};

    _visited_pair_of_states--; // we don't count the epsilon transition

    auto eps_result = check_for_virt_bisim(A_epsilon_const, A_trans, B_epsilon_const, B_trans, visited);

    if (eps_result->contradiction_free()) {
      return eps_result;
    }

    auto eps_cont = eps_result->get_contradictions();

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradiction =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _A->get_no_of_virtual_clocks() + 1);

    for (auto cur = eps_cont->begin(); cur < eps_cont->end(); cur++) {
      contradiction->append_zone(tchecker::vcg::revert_epsilon_trans(A_synced->zone(), A_epsilon->zone(), **cur));
      contradiction->append_zone(tchecker::vcg::revert_epsilon_trans(B_synced->zone(), B_epsilon->zone(), **cur));
    }

    contradiction->compress();

    // std::cout << __FILE__ << ": " << __LINE__ << ": emplace into non_bisim_cache (eps): " << std::endl;
    // std::cout << __FILE__ << ": " << __LINE__ << ": (" << _A->system().as_system_system().location(A_synced->vloc()[0])->name() << ", ";
    // std::cout << _B->system().as_system_system().location(B_synced->vloc()[0])->name() << ")" << std::endl;

    // for(auto vc : *contradiction) {
    //   tchecker::dbm::output_matrix(std::cout, vc->dbm(), vc->dim());
    // }

    _non_bisim_cache.emplace(A_synced, B_synced, contradiction);

    auto result = std::make_shared<algorithm_return_value_t>(syncer.revert_sync_with_urgent(A_state, B_state, contradiction), A_state, B_state);
    return result;
  }

  // check whether there already exists a contradiction
  auto cache = _non_bisim_cache.already_cached(A_synced, B_synced);
  if (!cache->is_empty()) {
    auto result = std::make_shared<algorithm_return_value_t>(syncer.revert_sync_with_urgent(A_state, B_state, cache), A_state, B_state);
    return result;
  }

  // checking whether normalized_pair is subset of visited. If not: add it.
  if (visited.check_and_add_pair(A_synced, B_synced)) {
    return std::make_shared<algorithm_return_value_t>(_A->get_no_of_virtual_clocks(), A_state, B_state);
  }

  // Since neither an epsilon transition is needed nor the pair is cached, we have to check the action transitions
  // first, find all events for which any of the symbolic states have outgoing transitions
  auto avail_events = std::make_shared<std::set<std::set<std::string>>>();

  _A->avail_events(avail_events, A_synced);
  _B->avail_events(avail_events, B_synced);


  for (auto & symbol : *avail_events) {

    std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A
      = std::make_shared<std::vector<tchecker::vcg::vcg_t::sst_t>>();
    std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B
      = std::make_shared<std::vector<tchecker::vcg::vcg_t::sst_t>>();

    _A->next_with_symbol(trans_A, A_synced, symbol);
    _B->next_with_symbol(trans_B, B_synced, symbol);

    std::shared_ptr<algorithm_return_value_t> return_from_transitions = check_for_outgoing_transitions(A_synced->zone(), B_synced->zone(), trans_A, trans_B, visited);

    if (!(return_from_transitions->contradiction_free())) {

      // performance improvement. We add all pairs of states that have an outgoing delay transition to one of the contradictions to the contradiction.
      auto enhanced_cont = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(return_from_transitions->get_contradictions()->dim());

      for(auto cur : *return_from_transitions->get_contradictions()) {
        std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> past = tchecker::virtual_constraint::factory(*cur);
        tchecker::dbm::open_down(past->dbm(), past->dim());
        std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_add = tchecker::virtual_constraint::factory(A_synced->zone(), _A->get_no_of_virtual_clocks());
        past->logic_and(to_add, *to_add);
        enhanced_cont->append_zone(to_add);        
      }

      // std::cout << __FILE__ << ": " << __LINE__ << ": emplace into non_bisim_cache (action): " << std::endl;
      // std::cout << __FILE__ << ": " << __LINE__ << ": (" << _A->system().as_system_system().location(A_synced->vloc()[0])->name() << ", ";
      // std::cout << _B->system().as_system_system().location(B_synced->vloc()[0])->name() << ")" << std::endl;

      // for(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc : *enhanced_cont) {
      //    tchecker::dbm::output_matrix(std::cout, vc->dbm(), vc->dim());
      // }
      _non_bisim_cache.emplace(A_synced, B_synced, enhanced_cont);

      auto result = std::make_shared<algorithm_return_value_t>(
                      syncer.revert_sync_with_urgent(A_state, B_state, enhanced_cont), A_state, B_state);

      return result;
    }
  }
  auto empty_contradictions_set =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _A->get_no_of_virtual_clocks() + 1);

  return std::make_shared<algorithm_return_value_t>(empty_contradictions_set, A_state, B_state);
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::extract_vc_without_contradictions(
    tchecker::zg::zone_t const & zone,
    std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions)
{

  assert(tchecker::dbm::is_tight(zone.dbm(), zone.dim()));
  assert(tchecker::dbm::is_consistent(zone.dbm(), zone.dim()));

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _A->get_no_of_virtual_clocks() + 1);

  result->append_zone(tchecker::virtual_constraint::factory(zone, _A->get_no_of_virtual_clocks()));

  // zone && not phi_1 && not phi_2 && not phi_3 && ...
  for (auto cur = contradictions->begin(); cur < contradictions->end(); cur++) {
    auto inter_result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
        _A->get_no_of_virtual_clocks() + 1);
    for (auto vc = result->begin(); vc < result->end(); vc++) {
      auto helper = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _A->get_no_of_virtual_clocks() + 1);
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

bool Lieb_et_al::do_an_epsilon_transition(tchecker::zg::state_sptr_t A_state, tchecker::zg::transition_sptr_t A_trans,
                                          tchecker::zg::state_sptr_t B_state, tchecker::zg::transition_sptr_t B_trans)
{

  assert(tchecker::vcg::are_zones_synced(A_state->zone(), B_state->zone(), _A->get_no_of_original_clocks(),
                                         _B->get_no_of_original_clocks()));

  tchecker::zg::state_sptr_t A_epsilon = _A->clone_state(A_state);
  tchecker::zg::state_sptr_t B_epsilon = _B->clone_state(B_state);

  if (tchecker::ta::delay_allowed(_A->system(), A_state->vloc())) {
    _A->semantics()->delay(A_epsilon->zone_ptr()->dbm(), A_epsilon->zone_ptr()->dim(), A_trans->tgt_invariant_container());
  }

  if (tchecker::ta::delay_allowed(_B->system(), B_state->vloc())) {
    _B->semantics()->delay(B_epsilon->zone_ptr()->dbm(), B_epsilon->zone_ptr()->dim(), B_trans->tgt_invariant_container());
  }

  return (A_state->zone() != A_epsilon->zone() || B_state->zone() != B_epsilon->zone());
}

bool check_disjointness(std::shared_ptr<tchecker::zone_container_t<tchecker::zg::zone_t>> zones)
{
  for (auto i = zones->begin(); i < zones->end(); i++) {
    for (auto j = i + 1; j < zones->end(); j++) {
      if (!tchecker::dbm::disjoint((*i)->dbm(), (*j)->dbm(), (*i)->dim())) {
        return false;
      }
    }
  }

  return true;
}

std::shared_ptr<algorithm_return_value_t> Lieb_et_al::check_target_pair(
    tchecker::zg::state_sptr_t target_state_A, tchecker::zg::transition_sptr_t trans_A,
    tchecker::zg::state_sptr_t target_state_B, tchecker::zg::transition_sptr_t trans_B,
    std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> already_found_contradictions,
    visited_map_t & visited, bool nondeterm)
{

  assert(tchecker::dbm::is_tight(target_state_A->zone().dbm(), target_state_A->zone().dim()));
  assert(tchecker::dbm::is_tight(target_state_B->zone().dbm(), target_state_B->zone().dim()));
  assert(target_state_A->zone().is_virtual_equivalent(target_state_B->zone(), _A->get_no_of_virtual_clocks()));

  tchecker::zg::state_sptr_t clone_A = _A->clone_state(target_state_A);
  tchecker::zg::state_sptr_t clone_B = _B->clone_state(target_state_B);

  auto without_cont = extract_vc_without_contradictions(target_state_A->zone(), already_found_contradictions);

  for (auto cur : *without_cont) {
    tchecker::zg::state_sptr_t clone_A = _A->clone_state(target_state_A);
    cur->logic_and(clone_A->zone(), target_state_A->zone());
    tchecker::zg::const_state_sptr_t final_A{clone_A};

    tchecker::zg::state_sptr_t clone_B = _B->clone_state(target_state_B);
    cur->logic_and(clone_B->zone(), target_state_B->zone());
    tchecker::zg::const_state_sptr_t final_B{clone_B};

    assert(tchecker::dbm::is_tight(final_A->zone().dbm(), final_A->zone().dim()));
    assert(tchecker::dbm::is_tight(final_B->zone().dbm(), final_B->zone().dim()));

    std::shared_ptr<algorithm_return_value_t> new_cont;

    if (nondeterm) {
      visited_map_t visited_copy{visited, _A, _B};
      new_cont = check_for_virt_bisim(final_A, trans_A, final_B, trans_B, visited_copy);
 
      if (new_cont->contradiction_free()) {
       visited.emplace(visited_copy);
      }
    } else {
      new_cont = check_for_virt_bisim(final_A, trans_A, final_B, trans_B, visited);
    }

    if (!new_cont->contradiction_free()) {      
      return new_cont;
    }
  }

  auto empty_contradictions_set =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _A->get_no_of_virtual_clocks() + 1);

  return std::make_shared<algorithm_return_value_t>(empty_contradictions_set, target_state_A->zone(), target_state_B->zone());
}

std::shared_ptr<algorithm_return_value_t>
Lieb_et_al::check_for_outgoing_transitions(tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                           std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A,
                                           std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B, visited_map_t & visited)
{

  assert(tchecker::dbm::is_tight(zone_A.dbm(), zone_A.dim()));
  assert(tchecker::dbm::is_tight(zone_B.dbm(), zone_B.dim()));

  if (0 == trans_A->size() && 0 == trans_B->size()) {
    return std::make_shared<algorithm_return_value_t>(_A->get_no_of_virtual_clocks(), zone_A, zone_B);
  }

  if (0 == trans_A->size() || 0 == trans_B->size()) {
    auto result = std::make_shared<algorithm_return_value_t>(_A->get_no_of_virtual_clocks(), zone_A, zone_B);

    for (auto && [status_A, s_A, t_A] : *trans_A) {
      auto target = tchecker::virtual_constraint::factory(s_A->zone(), _A->get_no_of_virtual_clocks());
      auto to_append = tchecker::vcg::revert_action_trans(zone_A, t_A->guard_container(), t_A->reset_container(),
                                                          t_A->tgt_invariant_container(), *target);
      result->add_to_contradictions(to_append);
    }

    for (auto && [status_B, s_B, t_B] : *trans_B) {
      auto target = tchecker::virtual_constraint::factory(s_B->zone(), _B->get_no_of_virtual_clocks());
      auto to_append = tchecker::vcg::revert_action_trans(zone_B, t_B->guard_container(), t_B->reset_container(),
                                                          t_B->tgt_invariant_container(), *target);
      result->add_to_contradictions(to_append);
    }

    return result;
  }

  tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> found_cont{trans_A->size(), trans_B->size(),
                                                                                         _A->get_no_of_virtual_clocks() + 1};
  std::vector<std::vector<bool>> finished(trans_A->size(),
                                          std::vector<bool>(trans_B->size(), false)); // init the finished matrix with false

  contradiction_searcher_t con_searcher{trans_A, trans_B, _A->get_no_of_virtual_clocks()};

  // we add an optimization here. We first check if the union of the targets of both sides are virtually equivalent. If they are
  // not, we can already stop here. We do this by running the search_contradiction function without any found contradiction and
  // checking, whether this already returns a contradiction.

  auto contradiction = con_searcher.search_contradiction(zone_A, zone_B, trans_A, trans_B, found_cont);

  if (!(contradiction->is_empty())) {
    return std::make_shared<algorithm_return_value_t>(contradiction, zone_A, zone_B);
  }

  do {
    for (size_t idx_A = 0; idx_A < trans_A->size(); idx_A++) {
      auto && [status_A, s_A, t_A] = (*trans_A)[idx_A];
      assert(tchecker::dbm::is_tight(s_A->zone().dbm(), s_A->zone().dim()));

      for (size_t idx_B = 0; idx_B < trans_B->size(); idx_B++) {
        if (finished[idx_A][idx_B]) {
          continue;
        }
        auto && [status_B, s_B, t_B] = (*trans_B)[idx_B];
        assert(tchecker::dbm::is_tight(s_B->zone().dbm(), s_B->zone().dim()));

        tchecker::zg::state_sptr_t s_A_constrained = _A->clone_state(s_A);
        auto s_A_vc = tchecker::virtual_constraint::factory(s_A->zone(), _A->get_no_of_virtual_clocks());
        tchecker::zg::state_sptr_t s_B_constrained = _B->clone_state(s_B);
        auto s_B_vc = tchecker::virtual_constraint::factory(s_B->zone(), _B->get_no_of_virtual_clocks());

        s_B_vc->logic_and(s_A_constrained->zone(), s_A_constrained->zone());
        s_A_vc->logic_and(s_B_constrained->zone(), s_B_constrained->zone());

        if (s_A_constrained->zone().is_empty() || s_B_constrained->zone().is_empty()) {
          continue;
        }

        auto new_cont = check_target_pair(s_A_constrained, t_A, s_B_constrained, t_B, 
                                          found_cont.get(idx_A, idx_B), visited, trans_A->size() > 1 && trans_B->size() > 1);

        if (new_cont->contradiction_free()) {
          finished[idx_A][idx_B] = true;
        }
        else {
          found_cont.get(idx_A, idx_B)->append_container(new_cont->get_contradictions());
          found_cont.get(idx_A, idx_B)->compress();
        }
      }
    }

    auto contradiction = con_searcher.search_contradiction(zone_A, zone_B, trans_A, trans_B, found_cont);

    if (!(contradiction->is_empty())) {
      return std::make_shared<algorithm_return_value_t>(contradiction, zone_A, zone_B);
    }

  } while (con_searcher.contradiction_still_possible(zone_A, zone_B, trans_A, trans_B, found_cont, finished));

  auto empty_contradictions_set =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _A->get_no_of_virtual_clocks() + 1);

  return std::make_shared<algorithm_return_value_t>(empty_contradictions_set, zone_A, zone_B);
}

} // end of namespace strong_timed_bisim

} // end of namespace tchecker
