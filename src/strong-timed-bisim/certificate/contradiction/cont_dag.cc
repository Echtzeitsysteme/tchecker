/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_dag.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

cont_dag_t::cont_dag_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2, 
                       tchecker::ta::state_t & first_init, tchecker::ta::state_t & second_init,
                       tchecker::clockval_t & first_clockval, tchecker::clockval_t & second_clockval,
                       tchecker::clock_constraint_container_t & first_invariant, tchecker::clock_constraint_container_t & second_invariant, 
                       std::size_t max_delay, tchecker::strong_timed_bisim::contra::cont_dag_t *previous)
    : base_graph_t(vcg1, vcg2), 
      _root(add_node(std::make_shared<node_t>(first_init, second_init,
                                     tchecker::clockval_factory(first_clockval),
                                     tchecker::clockval_factory(second_clockval),
                                     std::make_shared<tchecker::clock_constraint_container_t>(first_invariant), 
                                     std::make_shared<tchecker::clock_constraint_container_t>(second_invariant),
                                     max_delay,
                                     vcg1->get_no_of_original_clocks() + vcg2->get_no_of_original_clocks() < vcg1->get_no_of_virtual_clocks(),
                                     0, nullptr == previous
                                  ))),
      _delays(std::make_shared<std::vector<std::shared_ptr<delay_edge_t>>>()), _max_delay(max_delay),
      _urgent_clk_exists(vcg1->get_no_of_original_clocks() + vcg2->get_no_of_original_clocks() < vcg1->get_no_of_virtual_clocks()),
      _previous(previous)
{
  assert(vcg1->get_no_of_virtual_clocks() == vcg2->get_no_of_virtual_clocks());
}

bool cont_dag_t::create_cont_from_non_bisim_cache(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, 
                                                  std::shared_ptr<node_t> src)
{
  std::vector<std::shared_ptr<node_t>> to_process; // the nodes to process
  if(nullptr == src) {
    to_process.emplace_back(_root);
  } else {
    assert(nullptr != this->find_node(src));
    to_process.emplace_back(this->find_node(src));
  }

  while (!to_process.empty()) {
    auto cur = *(to_process.begin());
    if (cur->is_leaf(_vcg1, _vcg2)) {
      to_process.erase(to_process.begin());
    } else if (!cur->is_synchronized()) {
      auto new_sync = std::make_shared<node_t>(*cur);
      new_sync->synchronize();
      if (does_node_exist_in_this_or_upper(new_sync)) { // cycle detected?
        return false;
      }
      new_sync = this->add_node(new_sync);
      this->add_delay(0, cur, new_sync);
      to_process.erase(to_process.begin());
      to_process.emplace_back(new_sync);
    } else {
      auto max_delay = _max_delay;
      bool gate = std::any_of(std::next(cur->valuation().first->begin()), cur->valuation().first->end(),
                              [max_delay](const clock_rational_value_t & val) { return val < max_delay; });

      gate |= std::any_of(std::next(cur->valuation().second->begin()), cur->valuation().second->end(),
                          [max_delay](const clock_rational_value_t & val) { return val < max_delay; });

      std::pair<clock_rational_value_t, std::shared_ptr<node_t>> delay;
      if(gate) {
        delay = cur->max_delay(non_bisim_cache.entry(cur->location_pair()), _vcg1, _vcg2);
      }
      if (gate && delay.first > clock_rational_value_t(0, 1)) {
        if (does_node_exist_in_this_or_upper(delay.second)) { // cycle detected?
          return false;
        }
        auto new_node = this->add_node(delay.second);
        this->add_delay(delay.first, cur, new_node);
        to_process.erase(to_process.begin());
        to_process.emplace_back(new_node);
      } else {
        if (!add_non_bisim_action_transition(non_bisim_cache, cur)) {
          return false;
        }
        to_process.erase(to_process.begin());
      }
    }
  }
  return true;
}


void cont_dag_t::add_node_that_already_exists(std::shared_ptr<node_t> to_add)
{
  throw std::runtime_error(
    std::string(__FILE__) + ": " +
    std::to_string(__LINE__) + ": " +
    std::string("Trying to add a node to the Contradiction DAG that already exists.")
  );
}

std::shared_ptr<delay_edge_t> cont_dag_t::add_delay(std::shared_ptr<delay_edge_t> delay)
{
  _delays->push_back(delay);
  return delay;
}

std::ostream & cont_dag_t::dot_output(std::ostream & os, std::string const & name)
{
  tchecker::graph::dot_output_header(os, name);

  dot_output_helper(os);

  std::map<std::string, std::string> attr;

  for (std::shared_ptr<delay_edge_t> cur : *(delays())) {
    attr.clear();
    cur->attributes(attr);
    tchecker::graph::dot_output_edge(os, std::to_string(cur->src()->id()), std::to_string(cur->tgt()->id()), attr);
  }

  return tchecker::graph::dot_output_footer(os);
}

bool cont_dag_t::add_non_bisim_action_transition(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, std::shared_ptr<node_t> src)
{
  assert(!src->final());
  assert(nullptr != find_node(src));
  assert(src->is_synchronized());

  auto zones = src->generate_zones(_vcg1, _vcg2);

  auto state_1 = _vcg1->create_state(src->location_pair().first.vloc(), src->location_pair().first.intval(), *(zones->first));
  auto state_2 = _vcg2->create_state(src->location_pair().second.vloc(), src->location_pair().second.intval(), *(zones->second));

  std::shared_ptr<std::set<std::set<std::string>>> avail_events = std::make_shared<std::set<std::set<std::string>>>();
  _vcg1->avail_events(avail_events, state_1);  // since we assume that src cannot be a leaf, we know that the avail_events of both symb. states are the same

  for (auto & symbol : *avail_events) {
    std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_1 =
        std::make_shared<std::vector<tchecker::vcg::vcg_t::sst_t>>();
    std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_2 =
        std::make_shared<std::vector<tchecker::vcg::vcg_t::sst_t>>();

    _vcg1->next_with_symbol(trans_1, state_1, symbol);
    _vcg2->next_with_symbol(trans_2, state_2, symbol);

    // we now have all outgoing transitions of src labeled with symbol. We now analyze each pair of transitions.
    // In case for one transition all pairs return true, we have have found a valid sub Contradiction DAG. Otherwise, continue.

    std::vector<std::vector<bool>> return_values(trans_1->size(), std::vector<bool>(trans_2->size(), false));
    std::vector<std::vector<std::shared_ptr<cont_dag_t>>> sub_dag(trans_1->size(), std::vector<std::shared_ptr<cont_dag_t>>(trans_2->size(), nullptr));
   
    for (size_t idx_1 = 0; idx_1 < trans_1->size(); idx_1++) {
      auto && [status_1, s_1, t_1] = (*trans_1)[idx_1];
      for (size_t idx_2 = 0; idx_2 < trans_2->size(); idx_2++) {
        auto && [status_2, s_2, t_2] = (*trans_2)[idx_2];
        
        tchecker::ta::state_t first_ta_state{s_1->vloc_ptr(), s_1->intval_ptr()};
        tchecker::ta::state_t second_ta_state{s_2->vloc_ptr(), s_2->intval_ptr()};

        std::shared_ptr<cont_dag_t> cur_sub_dag 
            = std::make_shared<cont_dag_t>(_vcg1, _vcg2, first_ta_state, second_ta_state,
                                           *src->valuation().first, *src->valuation().second,
                                           t_1->tgt_invariant_container(), t_2->tgt_invariant_container(),
                                           _max_delay, this);
                    
        for(tchecker::clock_id_t i = 1; i < cur_sub_dag->_root->valuation().first->size(); i++) {
          if(satisfies(s_1->zone().dbm(), cur_sub_dag->_root->valuation().first->size(), i, 0, tchecker::ineq_cmp_t::LE, 0)) {
            cur_sub_dag->_root->reset_value(true, i);
          }
        }

        for(tchecker::clock_id_t i = 1; i < cur_sub_dag->_root->valuation().second->size(); i++) {
          if(satisfies(s_2->zone().dbm(), cur_sub_dag->_root->valuation().second->size(), i, 0, tchecker::ineq_cmp_t::LE, 0)) {
            cur_sub_dag->_root->reset_value(false, i);
          }
        }

        assert(cur_sub_dag->_root->is_element_of(s_1, s_2));

        // do we have to reset the urgent_clock?
        if(_urgent_clk_exists && 
            (!tchecker::ta::delay_allowed(_vcg1->system(), cur_sub_dag->_root->location_pair().first.vloc()) || 
             !tchecker::ta::delay_allowed(_vcg2->system(), cur_sub_dag->_root->location_pair().second.vloc()))) {
          cur_sub_dag->_root->reset_value(true, cur_sub_dag->_root->valuation().first->size() - 1);
          cur_sub_dag->_root->reset_value(false, cur_sub_dag->_root->valuation().second->size() - 1);
        }

        std::shared_ptr<node_t> synced_init_node 
          = std::make_shared<node_t>(*(cur_sub_dag->_root));

        synced_init_node->synchronize(); 
          
        if (does_node_exist_in_this_or_upper(synced_init_node)) { // cycle detected?
          continue;
        }

        if(non_bisim_cache.is_cached(synced_init_node->location_pair(), synced_init_node->valuation().first, _vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks())) {
          return_values[idx_1][idx_2] = cur_sub_dag->create_cont_from_non_bisim_cache(non_bisim_cache);
          sub_dag[idx_1][idx_2] = cur_sub_dag;
        }
      }
    }

    for (size_t idx_1 = 0; idx_1 < trans_1->size(); idx_1++) {
      if(std::all_of(return_values[idx_1].begin(), return_values[idx_1].end(), [](bool v){ return v; })) {
        for(std::size_t idx_2 = 0; idx_2 < trans_2->size(); idx_2++) {
          add_nodes_and_edges_of(*sub_dag[idx_1][idx_2], src, (*trans_1)[idx_1], (*trans_2)[idx_2]);
        }
        return true;
      }
    }

    for (size_t idx_2 = 0; idx_2 < trans_2->size(); idx_2++) {
      if(std::all_of(return_values.begin(), return_values.end(), [idx_2](const auto & row){ return row[idx_2]; })) {
        for(std::size_t idx_1 = 0; idx_1 < trans_1->size(); idx_1++) {
          add_nodes_and_edges_of(*sub_dag[idx_1][idx_2], src, (*trans_1)[idx_1], (*trans_2)[idx_2]);
        }
        return true;
      }
    } 
  }

  // we did not find a valid contradiction DAG. Therefore, return false
  return false;
}

void cont_dag_t::add_nodes_and_edges_of(cont_dag_t & other, std::shared_ptr<node_t> src, tchecker::zg::zg_t::sst_t trans_1, tchecker::zg::zg_t::sst_t trans_2)
{
  assert(nullptr != src);
  assert(nullptr != other._root);
  
  for(auto cur : *other._nodes) {
    if(nullptr == find_node(cur)) {
      add_node(cur);
    }
  }

  add_edge(*(std::get<2>(trans_1)), *(std::get<2>(trans_2)), src, other._root);

  for(auto cur : *other._edges) {
    assert(nullptr != cur->src());
    assert(nullptr != cur->tgt());
    auto edge_src = find_node(cur->src());
    auto edge_tgt = find_node(cur->tgt());
    add_edge(cur->edge_pair_ptr(), edge_src, edge_tgt);
  }

  for(auto cur : *other._delays) {
    assert(nullptr != cur->src());
    assert(nullptr != cur->tgt());
    auto edge_src = find_node(cur->src());
    auto edge_tgt = find_node(cur->tgt());
    add_delay(cur->delay(), edge_src, edge_tgt);
  }
}

bool cont_dag_t::does_node_exist_in_this_or_upper(std::shared_ptr<node_t> node)
{
  if (nullptr != find_node(node)) {
    return true;
  }
  if(nullptr != _previous) {
    return _previous->does_node_exist_in_this_or_upper(node);
  }
  return false;
}


} // end of namespace contra

} // end of namespace strong_timed_bisim

} // end of namespace tchecker