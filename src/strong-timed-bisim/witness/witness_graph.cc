/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/witness/witness_graph.hh"

#include "tchecker/dbm/dbm.hh"
#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/vcg/sync.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

graph_t::graph_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
    : _vcg1(vcg1), _vcg2(vcg2), _nodes(std::make_shared<std::vector<std::shared_ptr<node_t>>>()),
      _edges(std::make_shared<std::vector<std::shared_ptr<edge_t>>>()), _nodes_id_counter(0)
{
}

void graph_t::add_node(tchecker::zg::state_sptr_t A_node, tchecker::zg::state_sptr_t B_node, bool initial)
{
  assert(A_node->zone().is_virtual_equivalent(B_node->zone(), _vcg1->get_no_of_virtual_clocks()));

  std::shared_ptr<node_t> found = find_node(A_node, B_node);

  if (nullptr == found) {
    std::shared_ptr<node_t> to_add =
        std::make_shared<node_t>(A_node, B_node, _nodes_id_counter++, _vcg1->get_no_of_virtual_clocks(), initial);
    _nodes->push_back(to_add);
  }
  else {
    found->add_zone(tchecker::virtual_constraint::factory(A_node->zone(), _vcg1->get_no_of_virtual_clocks()));
  }

  assert(nullptr != find_node(A_node, B_node));
}

void graph_t::add_edge(tchecker::zg::state_sptr_t A_source, tchecker::ta::state_t & A_target,
                       tchecker::zg::transition_t & A_transition, tchecker::zg::state_sptr_t B_source,
                       tchecker::ta::state_t & B_target, tchecker::zg::transition_t & B_transition,
                       std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition)
{
  assert(A_source->zone().is_virtual_equivalent(B_source->zone(), _vcg1->get_no_of_virtual_clocks()));
  assert(tchecker::vcg::are_zones_synced(A_source->zone(), B_source->zone(), _vcg1->get_no_of_original_clocks(),
                                         _vcg2->get_no_of_original_clocks()));

  std::shared_ptr<node_t> src = find_node(A_source, B_source);

  // src MUST be element of nodes
  assert(nullptr != src);

  auto empty_target =
      tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>(condition->get_no_of_virtual_clocks());
  add_node(A_target, B_target, empty_target);

  std::shared_ptr<node_t> target = find_node(A_target, B_target);

  std::shared_ptr<edge_t> to_add = std::make_shared<edge_t>(A_transition, B_transition, src, target, condition);

  _edges->push_back(to_add);
}

void graph_t::attributes(tchecker::strong_timed_bisim::witness::node_t const & n, std::map<std::string, std::string> & m) const
{
  std::map<std::string, std::string> first_map;
  tchecker::ta::attributes(_vcg1->system(), n.location_pair_ptr()->first, first_map);

  std::map<std::string, std::string> second_map;
  tchecker::ta::attributes(_vcg2->system(), n.location_pair_ptr()->second, first_map);

  for (const auto & [key, value] : first_map) {
    m["first_" + key] = value;
  }

  for (const auto & [key, value] : second_map) {
    m["second_" + key] = value;
  }
}

void graph_t::attributes(tchecker::strong_timed_bisim::witness::edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["first_vedge"] = tchecker::to_string(e.type()->edge_pair_ptr()->first.vedge(), _vcg1->system().as_system_system());
  m["second_vedge"] = tchecker::to_string(e.type()->edge_pair_ptr()->second.vedge(), _vcg2->system().as_system_system());

  // find maximum clock id of first system
  tchecker::clock_id_t max = 0;

  for (const auto & [key, value] : _vcg1->system().clock_variables().flattened().index()) {
    if (key > max) {
      max = key;
    }
  }

  // adding the "0" clock.
  max++;

  auto find_clock_name = [&](tchecker::clock_id_t id) {
    if (0 == id) {
      return std::string("0");
    }

    if (id < max) {
      return _vcg1->system().clock_variables().flattened().index().value(id - 1);
    }
    else {
      return _vcg2->system().clock_variables().flattened().index().value(id - max - 1);
    }
  };

  std::stringstream sstream;
  tchecker::dbm::output(sstream, e.condition()->dbm(), e.condition()->dim(), find_clock_name);

  //  tchecker::dbm::output_matrix(sstream, e.condition()->dbm(), e.condition()->dim());
  m["condition"] = sstream.str();
}

std::shared_ptr<node_t> graph_t::find_node(tchecker::zg::state_sptr_t A_node, tchecker::zg::state_sptr_t B_node)
{
  std::shared_ptr<node_t> to_add =
      std::make_shared<node_t>(A_node, B_node, _nodes_id_counter++, _vcg1->get_no_of_virtual_clocks(), false);

  for (auto cur : *_nodes) {
    if (cur->equal_location_pair(*to_add)) {
      return cur;
    }
  }

  return nullptr;
}

std::shared_ptr<node_t> graph_t::find_node(tchecker::ta::state_t & first, tchecker::ta::state_t & second)
{
  std::shared_ptr<node_t> to_add =
      std::make_shared<node_t>(first, second, _vcg1->get_no_of_virtual_clocks(), _nodes_id_counter++);

  for (auto cur : *_nodes) {
    if (cur->equal_location_pair(*to_add)) {
      return cur;
    }
  }

  return nullptr;
}

void graph_t::create_witness_from_visited(tchecker::strong_timed_bisim::visited_map_t & visited,
                                          tchecker::zg::state_sptr_t first_init, tchecker::zg::state_sptr_t second_init)
{
  add_node(first_init, second_init, true);

  for (auto & [key, value] : visited) {

    tchecker::ta::state_t first_non_const = key.first;
    tchecker::ta::state_t second_non_const = key.second;

    add_node(first_non_const, second_non_const, *value);

    for (auto & cur : *value) {

      auto first = create_symbolic_state(first_non_const, *cur, first_init, true);
      auto second = create_symbolic_state(second_non_const, *cur, second_init, false);
      tchecker::zg::const_state_sptr_t first_const{first};
      tchecker::zg::const_state_sptr_t second_const{second};


      std::vector<tchecker::vcg::vcg_t::sst_t> v_1, v_2;
      _vcg1->next(first_const, v_1);
      _vcg2->next(first_const, v_2);

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
          for (auto cur_2 : trans_2) {
            auto && [status_2, s_2, t_2] = *cur_2;
            auto node = find_node(s_1, s_2);
            if(nullptr == node) {
              //add_node(s_1, s_2)
            }
          }
        }
      }
    }
  }
}

void graph_t::edge_cleanup()
{
  std::map<edge_type_t, std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> map;

  for (auto cur : *_edges) {
    if (0 == map.count(*(cur->type()))) {
      map[*(cur->type())] = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
          _vcg1->get_no_of_virtual_clocks() + 1);
    }

    map[*(cur->type())]->append_zone(cur->condition());
  }

  std::shared_ptr<std::vector<std::shared_ptr<edge_t>>> new_edges = std::make_shared<std::vector<std::shared_ptr<edge_t>>>();

  for (const auto & entry : map) {
    entry.second->compress();
    for (auto cond : *(entry.second)) {
      std::shared_ptr<edge_t> to_add = std::make_shared<edge_t>(std::make_shared<edge_type_t>(entry.first), cond);
      new_edges->push_back(to_add);
    }
  }

  _edges = new_edges;
}

void graph_t::add_node(tchecker::ta::state_t & first, tchecker::ta::state_t & second,
                       tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & vcs)
{
  std::shared_ptr<node_t> found = find_node(first, second);

  if (nullptr == found) {
    auto to_add = std::make_shared<node_t>(first, second, vcs, _nodes_id_counter++);
    _nodes->push_back(to_add);
  }
  else {
    found->add_zones(vcs);
  }
}

void graph_t::add_node(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair,
                       tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & vcs)
{
  add_node(loc_pair.first, loc_pair.second, vcs);
}

std::shared_ptr<std::set<edge_type_t>> graph_t::find_all_edge_types()
{
  std::shared_ptr<std::set<edge_type_t>> result = std::make_shared<std::set<edge_type_t>>();

  for (auto cur : *_edges) {
    result->insert(*cur->type());
  }

  return result;
}

tchecker::zg::state_sptr_t graph_t::create_symbolic_state(tchecker::ta::state_t & ta_state,
                                                          tchecker::virtual_constraint::virtual_constraint_t & vc,
                                                          tchecker::zg::state_sptr_t & init, bool first_not_second)
{
  // this method is an ugly hack due to this disaster called "memory management" implemented in tchecker
  // todo: f***ing fix this!
  auto result = first_not_second ? _vcg1->clone_state(init) : _vcg2->clone_state(init);

  result->_vloc = ta_state.vloc_ptr();
  result->_intval = ta_state.intval_ptr();

  std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>> zones =
      vc.generate_synchronized_zones(_vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks());

  assert(result->zone().dim() == first_not_second ? zones.first->dim() : zones.second->dim());
  tchecker::dbm::copy(result->zone().dbm(), first_not_second ? zones.first->dbm() : zones.second->dbm(), result->zone().dim());

  return result;
}

std::ostream & dot_output(std::ostream & os, graph_t & g, std::string const & name)
{

  g.edge_cleanup();

  std::map<std::string, std::string> attr;

  tchecker::graph::dot_output_header(os, name);

  for (std::shared_ptr<node_t> node : *(g.nodes())) {
    attr.clear();
    g.attributes(*node, attr);
    tchecker::graph::dot_output_node(os, std::to_string(node->id()), attr); // ToDo: check again!
  }

  for (std::shared_ptr<edge_t> edge : *(g.edges())) {
    attr.clear();
    g.attributes(*edge, attr);
    tchecker::graph::dot_output_edge(os, std::to_string(edge->type()->src()->id()), std::to_string(edge->type()->tgt()->id()),
                                     attr); // ToDo: check again!
  }

  tchecker::graph::dot_output_footer(os);

  return os;
}

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker