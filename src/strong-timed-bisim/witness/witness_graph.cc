/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/witness/witness_graph.hh"

#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/vcg/sync.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

graph_t::graph_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2) : 
                _vcg1(vcg1), _vcg2(vcg2), 
                _nodes(std::make_shared<std::vector<std::shared_ptr<node_t>>>()), 
                _edges(std::make_shared<std::vector<std::shared_ptr<edge_t>>>()),
                _nodes_id_counter(0)
{
}

void graph_t::add_node(tchecker::zg::state_sptr_t A_node, tchecker::zg::state_sptr_t B_node) 
{
  assert(A_node->zone().is_virtual_equivalent(B_node->zone(), _vcg1->get_no_of_virtual_clocks()));

  std::shared_ptr<node_t> found = find_node(A_node, B_node);

  if (nullptr == found) {
    std::shared_ptr<node_t> to_add = std::make_shared<node_t>(A_node, B_node, _nodes_id_counter++, _vcg1->get_no_of_virtual_clocks());
    _nodes->push_back(to_add);
  } else {
    found->add_zone(tchecker::virtual_constraint::factory(A_node->zone(), _vcg1->get_no_of_virtual_clocks()));
  }

  assert(nullptr != find_node(A_node, B_node));
}

void graph_t::add_edge(tchecker::zg::state_sptr_t A_source, tchecker::zg::state_sptr_t A_target, tchecker::zg::transition_t & A_transition, 
                       tchecker::zg::state_sptr_t B_source, tchecker::zg::state_sptr_t B_target, tchecker::zg::transition_t & B_transition,
                       std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition)
{
  assert(A_source->zone().is_virtual_equivalent(B_source->zone(), _vcg1->get_no_of_virtual_clocks()));
  assert(A_target->zone().is_virtual_equivalent(B_target->zone(), _vcg1->get_no_of_virtual_clocks()));
  assert(tchecker::vcg::are_zones_synced(A_source->zone(), B_source->zone(), _vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks()));
  assert(tchecker::vcg::are_zones_synced(A_target->zone(), B_target->zone(), _vcg1->get_no_of_original_clocks(), _vcg2->get_no_of_original_clocks()));

  std::shared_ptr<node_t> src = find_node(A_source, B_source);

  // src MUST be element of nodes
  assert(nullptr != src);

  add_node(A_target, B_target);
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

  for (const auto& [key, value] : first_map) {
    m["first_" + key] = value;
  }

  for (const auto& [key, value] : second_map) {
    m["second_" + key] = value;
  }
}

void graph_t::attributes(tchecker::strong_timed_bisim::witness::edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["first_vedge"] = tchecker::to_string(e.type()->edge_pair_ptr()->first.vedge(), _vcg1->system().as_system_system());
  m["second_vedge"] = tchecker::to_string(e.type()->edge_pair_ptr()->second.vedge(), _vcg2->system().as_system_system());

  // find maximum clock id of first system
  tchecker::clock_id_t max = 0;

  for(const auto& [key, value] : _vcg1->system().clock_variables().flattened().index()) {
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
    } else {
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
  std::shared_ptr<node_t> to_add = std::make_shared<node_t>(A_node, B_node, _nodes_id_counter++, _vcg1->get_no_of_virtual_clocks());

  for(auto cur : *_nodes) {
    if(cur->equal_location_pair(*to_add)) {
      return cur;
    }
  }

  return nullptr;
}

void graph_t::edge_cleanup()
{
  std::map<edge_type_t, std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> map;

  for (auto cur : *_edges) {
    if(0 == map.count(*(cur->type()))) {
      map[*(cur->type())] = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
                                _vcg1->get_no_of_virtual_clocks() + 1);
    }

    map[*(cur->type())]->append_zone(cur->condition());
  }

  std::shared_ptr<std::vector<std::shared_ptr<edge_t>>> new_edges = std::make_shared<std::vector<std::shared_ptr<edge_t>>>();

  for(const auto &entry : map) {
    entry.second->compress();
    for(auto cond : *(entry.second)) {
      std::shared_ptr<edge_t> to_add = std::make_shared<edge_t>(std::make_shared<edge_type_t>(entry.first), cond);
      new_edges->push_back(to_add);
    }
  }

  _edges = new_edges;
}

std::shared_ptr<std::set<edge_type_t>> graph_t::find_all_edge_types()
{
  std::shared_ptr<std::set<edge_type_t>> result = std::make_shared<std::set<edge_type_t>>();

  for (auto cur : *_edges) {
    result->insert(*cur->type());
  }

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
    tchecker::graph::dot_output_edge(os, std::to_string(edge->type()->src()->id()), std::to_string(edge->type()->tgt()->id()), attr); // ToDo: check again!
  }

  tchecker::graph::dot_output_footer(os);

  return os;
}


} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker