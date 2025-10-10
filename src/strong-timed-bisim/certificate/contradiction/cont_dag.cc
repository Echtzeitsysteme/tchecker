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
                       tchecker::zg::state_sptr_t first_init, tchecker::zg::state_sptr_t second_init,
                       clock_rational_value_t max_delay)
    : base_graph_t(vcg1, vcg2), _init_states(std::make_pair(first_init, second_init)), 
      _delays(std::make_shared<std::vector<std::shared_ptr<delay_edge_t>>>()), _max_delay(max_delay + 1),
      _urgent_clk_exists(_vcg1->get_no_of_original_clocks() + _vcg2->get_no_of_original_clocks() < _vcg1->get_no_of_virtual_clocks())
{
  assert(vcg1->get_no_of_virtual_clocks() == vcg2->get_no_of_virtual_clocks());
}

cont_dag_t::cont_dag_t(const cont_dag_t & other, tchecker::zg::state_sptr_t first_init, tchecker::zg::state_sptr_t second_init)
    : base_graph_t(other.vcg1_ptr(), other.vcg2_ptr()), _init_states(std::make_pair(first_init, second_init)), 
      _delays(std::make_shared<std::vector<std::shared_ptr<delay_edge_t>>>()), _max_delay(other._max_delay),
      _urgent_clk_exists(other._urgent_clk_exists)
{
  for(auto cur : *(other._nodes)) {
    add_node(cur);
  }
}

bool cont_dag_t::create_cont_from_non_bisim_cache(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, 
                                                  std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, 
                                                  std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2)
{

  auto init_node = std::make_shared<node_t>(_init_states.first, _init_states.second, _vcg1->get_no_of_original_clocks(),
                                             _vcg2->get_no_of_original_clocks(), invariant_1, invariant_2, 0, _urgent_clk_exists, true);

  return create_cont_from_non_bisim_cache(non_bisim_cache, *init_node);
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

bool cont_dag_t::create_cont_from_non_bisim_cache(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, node_t &src)
{
  std::vector<std::shared_ptr<node_t>> to_process; // the nodes to process

  auto init_node = this->add_node(src);
  this->_root = init_node;
  to_process.emplace_back(init_node);

  while (!to_process.empty()) {
    auto cur = *(to_process.begin());
    if (cur->is_leaf(_init_states.first, _init_states.second, _vcg1, _vcg2, _max_delay)) {
      to_process.erase(to_process.begin());
    }
    else if (!cur->is_synchronized()) {
      auto new_sync = std::make_shared<node_t>(*cur);
      new_sync->synchronize();
      if (nullptr != find_node(new_sync)) { // cycle detected?
        return false;
      }
      new_sync = this->add_node(*new_sync);
      this->add_delay(0, cur, new_sync);
      to_process.erase(to_process.begin());
      to_process.emplace_back(new_sync);
    }
    else {
      auto max_delay = _max_delay;
      bool gate = std::any_of(std::next(cur->valuation().first->begin()), cur->valuation().first->end(),
                              [max_delay](const clock_rational_value_t & val) { return val < max_delay; });

      gate |= std::any_of(std::next(cur->valuation().second->begin()), cur->valuation().second->end(),
                          [max_delay](const clock_rational_value_t & val) { return val < max_delay; });

      std::pair<clock_rational_value_t, std::shared_ptr<node_t>> delay;
      if(gate) {
        delay = cur->max_delay(non_bisim_cache.entry(cur->location_pair()), _max_delay, _vcg1, _vcg2);
      }
      if (gate && delay.first > clock_rational_value_t(0, 1)) {
        if (nullptr != find_node(delay.second)) { // cycle detected?
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

bool cont_dag_t::add_non_bisim_action_transition(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, std::shared_ptr<node_t> src)
{
  assert(!src->final());

  auto zones = src->generate_zones(_vcg1, _vcg2);

  auto state_1 = _vcg1->clone_state(_init_states.first);
  tchecker::dbm::copy(state_1->zone().dbm(), zones->first->dbm(), zones->first->dim());

  auto state_2 = _vcg2->clone_state(_init_states.second);
  tchecker::dbm::copy(state_2->zone().dbm(), zones->second->dbm(), zones->second->dim());

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
        assert(s_2->zone().is_virtual_equivalent(s_1->zone(), _vcg1->get_no_of_virtual_clocks()));
        
        std::shared_ptr<cont_dag_t> cur_sub_dag = std::make_shared<cont_dag_t>(*this, s_1, s_2);
        std::shared_ptr<node_t> cur_init_node 
          = std::make_shared<node_t>(s_1, s_2, _vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks(),
                                      std::make_shared<tchecker::clock_constraint_container_t>(t_1->tgt_invariant_container()), 
                                      std::make_shared<tchecker::clock_constraint_container_t>(t_2->tgt_invariant_container()), 
                                      0, _urgent_clk_exists, false);
                    
        cur_init_node->set_valuation(src->valuation());

        for(tchecker::clock_id_t i = 1; i < cur_init_node->valuation().first->size(); i++) {
          if(satisfies(s_1->zone().dbm(), cur_init_node->valuation().first->size(), i, 0, tchecker::ineq_cmp_t::LE, 0)) {
            cur_init_node->reset_value(true, i);
          }
        }

        for(tchecker::clock_id_t i = 1; i < cur_init_node->valuation().second->size(); i++) {
          if(satisfies(s_2->zone().dbm(), cur_init_node->valuation().second->size(), i, 0, tchecker::ineq_cmp_t::LE, 0)) {
            cur_init_node->reset_value(false, i);
          }
        }

        assert(cur_init_node->is_element_of(s_1, s_2));
        // do we have to reset the urgent_clock?
        if(_urgent_clk_exists && 
            (!tchecker::ta::delay_allowed(_vcg1->system(), cur_init_node->location_pair().first.vloc()) || 
             !tchecker::ta::delay_allowed(_vcg2->system(), cur_init_node->location_pair().second.vloc()))) {
          cur_init_node->reset_value(true, cur_init_node->valuation().first->size() - 1);
          cur_init_node->reset_value(false, cur_init_node->valuation().second->size() - 1);
        }

        std::shared_ptr<node_t> synced_init_node 
          = std::make_shared<node_t>(*cur_init_node);

        synced_init_node->synchronize(); 
          
        if (nullptr != find_node(synced_init_node)) { // cycle detected?
          continue;
        }

        if(non_bisim_cache.is_cached(synced_init_node->location_pair(), synced_init_node->valuation().first, _vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks())) {
          return_values[idx_1][idx_2] = cur_sub_dag->create_cont_from_non_bisim_cache(non_bisim_cache, *cur_init_node);
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

  // we did not find a valid contradiction DAG. Therefore, return nullptr
  return false;
}

void cont_dag_t::add_nodes_and_edges_of(cont_dag_t & other, std::shared_ptr<node_t> src, tchecker::zg::zg_t::sst_t trans_1, tchecker::zg::zg_t::sst_t trans_2)
{
  add_edge(*(std::get<2>(trans_1)), *(std::get<2>(trans_2)), src, other._root);
  for(auto cur : *other._nodes) {
    if(nullptr == find_node(cur)) {
      add_node(cur);
    }
  }

  for(auto cur : *other._edges) {
    auto src = find_node(cur->src());
    auto tgt = find_node(cur->tgt());
    add_edge(cur->edge_pair_ptr(), src, tgt);
  }

  for(auto cur : *other._delays) {
    auto src = find_node(cur->src());
    auto tgt = find_node(cur->tgt());
    add_delay(cur->delay(), src, tgt);
  }
}


} // end of namespace contra

} // end of namespace strong_timed_bisim

} // end of namespace tchecker