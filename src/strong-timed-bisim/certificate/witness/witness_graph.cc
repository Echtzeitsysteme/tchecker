/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/witness/witness_graph.hh"

#include "tchecker/strong-timed-bisim/certificate/certificate_edge.hh"

#include "tchecker/dbm/dbm.hh"
#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/vcg/sync.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

graph_t::graph_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
    : base_graph_t(vcg1, vcg2)
{
}

void graph_t::add_edge(tchecker::zg::state_sptr_t A_source, tchecker::ta::state_t & A_target,
                       tchecker::zg::transition_t & A_transition, tchecker::zg::state_sptr_t B_source,
                       tchecker::ta::state_t & B_target, tchecker::zg::transition_t & B_transition,
                       std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition)
{
  assert(A_source->zone().is_virtual_equivalent(B_source->zone(), _vcg1->get_no_of_virtual_clocks()));
  assert(tchecker::vcg::are_zones_synced(A_source->zone(), B_source->zone(), _vcg1->get_no_of_original_clocks(),
                                         _vcg2->get_no_of_original_clocks()));

  std::shared_ptr<node_t> src = find_node(A_source, B_source, _vcg1->get_no_of_virtual_clocks());

  // src MUST be element of nodes
  assert(nullptr != src);

  auto empty_target =
      std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> (condition->get_no_of_virtual_clocks() + 1);
  
  std::shared_ptr<node_t> target = find_node(A_target, B_target, empty_target);

  if(nullptr == target) {
    add_node(A_target, B_target, empty_target);
    target = find_node(A_target, B_target, empty_target);
  }

  base_graph_t::add_edge(A_transition, B_transition, src, target, condition);
}

void graph_t::create_witness_from_visited(tchecker::strong_timed_bisim::visited_map_t & visited,
                                          tchecker::zg::state_sptr_t first_init, tchecker::zg::state_sptr_t second_init)
{
  add_node(first_init, second_init, _vcg1->get_no_of_virtual_clocks(), 0, true);

  for (auto & [key, value] : visited) {

    tchecker::ta::state_t first_non_const = key.first;
    tchecker::ta::state_t second_non_const = key.second;

    add_node(first_non_const, second_non_const, value);

    for (auto & cur : *value) {

      auto first = create_symbolic_state(first_non_const, *cur, first_init, true);
      auto second = create_symbolic_state(second_non_const, *cur, second_init, false);
      tchecker::zg::const_state_sptr_t first_const{first};
      tchecker::zg::const_state_sptr_t second_const{second};


      std::vector<tchecker::vcg::vcg_t::sst_t> v_1, v_2;
      _vcg1->next(first_const, v_1);
      _vcg2->next(second_const, v_2);

      auto add_to_avail_events = [](std::set<std::set<std::string>> & avail_events,
                                    std::vector<tchecker::vcg::vcg_t::sst_t> & v, const tchecker::ta::system_t & system) {
        for (auto && [status, s, t] : v) {
          avail_events.insert(t->vedge().event_names(system));
        }
      };

      // find all events for which any of the symbolic states have outgoing transitions
      std::set<std::set<std::string>> avail_events;

      add_to_avail_events(avail_events, v_1, _vcg1->system());
      add_to_avail_events(avail_events, v_2, _vcg2->system());

      for (auto & symbol : avail_events) {
        auto add_to_transition_list = [](std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_list,
                                         std::vector<tchecker::vcg::vcg_t::sst_t> & v, const tchecker::ta::system_t & system,
                                         std::set<std::string> symbol) {
          for (auto & cur_trans : v) {
            assert(tchecker::dbm::is_tight(std::get<1>(cur_trans)->zone().dbm(), std::get<1>(cur_trans)->zone().dim()));
            if (std::get<2>(cur_trans)->vedge().event_equal(system, symbol)) {
              trans_list.emplace_back(&cur_trans);
            }
          }
        };

        std::vector<tchecker::vcg::vcg_t::sst_t *> trans_1;
        std::vector<tchecker::vcg::vcg_t::sst_t *> trans_2;

        add_to_transition_list(trans_1, v_1, _vcg1->system(), symbol);
        add_to_transition_list(trans_2, v_2, _vcg2->system(), symbol);

        for (auto cur_1 : trans_1) {
          auto && [status_1, s_1, t_1] = *cur_1;
          auto tgt_loc_1 = tchecker::ta::state_t(s_1->vloc_ptr(), s_1->intval_ptr());
          auto cond_1 = tchecker::virtual_constraint::factory(s_1->zone(), _vcg1->get_no_of_virtual_clocks());

          for (auto cur_2 : trans_2) {
            auto && [status_2, s_2, t_2] = *cur_2;
            auto tgt_loc_2 = tchecker::ta::state_t(s_2->vloc_ptr(), s_2->intval_ptr());
            auto cond_2 = tchecker::virtual_constraint::factory(s_2->zone(), _vcg2->get_no_of_virtual_clocks());

            std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> cond
              = tchecker::virtual_constraint::factory(_vcg1->get_no_of_virtual_clocks());

            cond_2->logic_and(cond, *cond_1);
                        
            add_edge(first, tgt_loc_1, *t_1, 
                    second, tgt_loc_2, *t_2, cond);

          }
        }
      }
    }
  }
}

void graph_t::edge_cleanup()
{
  std::map<base_edge_t, std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> map;

  for (auto cur : *_edges) {
    if(cur->src()->empty() || cur->tgt()->empty()) {
      continue;
    }

    if (0 == map.count(*(cur))) {
      map[*(cur)] = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _vcg1->get_no_of_virtual_clocks() + 1);
    }

    map[*(cur)]->append_zone(cur->condition());
  }

  std::shared_ptr<std::vector<std::shared_ptr<edge_t>>> new_edges = std::make_shared<std::vector<std::shared_ptr<edge_t>>>();

  for (const auto & entry : map) {
    entry.second->compress();
    for (auto cond : *(entry.second)) {
      std::shared_ptr<edge_t> to_add = std::make_shared<edge_t>(std::make_shared<base_edge_t>(entry.first), cond);
      new_edges->push_back(to_add);
    }
  }

  _edges = new_edges;
}

void graph_t::node_cleanup() {

  // run compress on each node
  std::for_each(_nodes->begin(), _nodes->end(), 
    [](const std::shared_ptr<node_t>& ptr) {
      ptr->compress();
    });

  edge_cleanup();
  
  // delete nodes if they are empty
  _nodes->erase(std::remove_if(_nodes->begin(), _nodes->end(),
                              [](const std::shared_ptr<node_t>& node) {
                                return node->empty();
                              }),
                _nodes->end());
}

void graph_t::add_node_that_already_exists(std::shared_ptr<node_t> to_add)
{
  auto found = find_node(to_add);
  assert(nullptr != found);
  found->add_zones(*(to_add->zones()));
}

std::shared_ptr<std::set<base_edge_t>> graph_t::find_all_edge_types()
{
  std::shared_ptr<std::set<base_edge_t>> result = std::make_shared<std::set<base_edge_t>>();

  for (auto cur : *_edges) {
    result->insert(*cur);
  }

  return result;
}

tchecker::zg::state_sptr_t graph_t::create_symbolic_state(tchecker::ta::state_t & ta_state,
                                                          tchecker::virtual_constraint::virtual_constraint_t & vc,
                                                          tchecker::zg::state_sptr_t & init, bool first_not_second)
{
  // Given a ta state and a zone, I do not understand how to create a zg::state_t. Therefore: Ugly Hack.
  auto result = first_not_second ? _vcg1->clone_state(init) : _vcg2->clone_state(init);

  result->_vloc = ta_state.vloc_ptr();
  result->_intval = ta_state.intval_ptr();

  std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>> zones =
      vc.generate_synchronized_zones(_vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks());

  assert(result->zone().dim() == first_not_second ? zones.first->dim() : zones.second->dim());
  tchecker::dbm::copy(result->zone().dbm(), first_not_second ? zones.first->dbm() : zones.second->dbm(), result->zone().dim());

  return result;
}

std::ostream & graph_t::dot_output(std::ostream & os, std::string const & name)
{
  edge_cleanup();
  node_cleanup();
  return base_graph_t::dot_output(os, name);
}

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker