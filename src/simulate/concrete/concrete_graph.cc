/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/concrete/concrete_graph.hh"

#include "tchecker/simulate/concrete/clock_names.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/operational-semantics/zone_valuation_converter.hh"

namespace tchecker {

namespace simulate {

namespace concrete {

/* node_t */

node_t::node_t(std::shared_ptr<tchecker::ta::state_t> ta_state, std::shared_ptr<tchecker::clockval_t> valuation, size_t id,
               bool initial, bool final)
  : _flags(initial, final), _ta_state(ta_state), _valuation(valuation), _id(id)
{
}

node_t::node_t(std::shared_ptr<tchecker::ta::state_t> ta_state, tchecker::zg::zone_t & zone, size_t id,
               bool initial, bool final)
  : node_t(ta_state, tchecker::operational_semantics::convert(zone), id, initial, final)
{
}

node_t::node_t(node_t const & other)
  : _flags(other._flags), _ta_state(other._ta_state), _valuation(clockval_clone(*other._valuation), &clockval_destruct_and_deallocate), _id(other._id)
{
}

void node_t::attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const
{
  tchecker::graph::attributes(this->_flags, m);
  tchecker::ta::attributes(system, *(this->_ta_state), m);
  m["clockval"] = to_string(*_valuation, clock_names(system));
}

tchecker::graph::node_flags_t node_t::flags() const {return _flags;}

std::size_t node_t::id() const {return _id;}

void node_t::set_id(std::size_t id) {_id = id;}

std::shared_ptr<tchecker::clockval_t> node_t::valuation() const {return _valuation;}

std::shared_ptr<tchecker::ta::state_t> node_t::ta_state() const {return _ta_state;}


/* edge_t */

edge_t::edge_t(node_t & src, node_t & tgt)
  : _src(src), _tgt(tgt)
{
}

node_t edge_t::src() {return _src;}
  
node_t edge_t::tgt() {return _tgt;}

/*! action_edge_t */

action_edge_t::action_edge_t(tchecker::zg::zg_t::transition_t & t, node_t & src, node_t & tgt)
  : edge_t(src, tgt), _ta_trans(t)
{
}

void action_edge_t::attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(system, *(this->_ta_trans), m);
}

/*! delay_edge_t */

delay_edge_t::delay_edge_t(tchecker::clock_rational_value_t & delay, node_t & src, node_t & tgt)
  : edge_t(src, tgt), _delay(delay)
{
}

void delay_edge_t::attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const
{
  m["delay"] = to_string(_delay);
}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::ta::system_t const> system)
  : _system(system)
{
}

void graph_t::dot_output(std::ostream & os, std::string const & name) {

  tchecker::graph::dot_output_header(os, name);


  std::map<std::string, std::string> attr;

  for (auto cur : _nodes) {
    attr.clear();
    cur->attributes(*_system, attr);
    tchecker::graph::dot_output_node(os, std::to_string(cur->id()), attr);
  }

  for (auto cur : _edges) {
    attr.clear();
    cur->attributes(*_system, attr);
    tchecker::graph::dot_output_edge(os, std::to_string(cur->src().id()), std::to_string(cur->tgt().id()), attr);
  }
}

std::shared_ptr<node_t> graph_t::add_node(std::shared_ptr<tchecker::ta::state_t> ta_state, std::shared_ptr<tchecker::clockval_t> valuation,
                              bool initial, bool final)
{
  std::size_t id = _nodes.size();
  std::shared_ptr<node_t> result = std::make_shared<node_t>(ta_state, valuation, id, initial, final);
  _nodes.emplace_back(result);
  return result;
}

std::shared_ptr<node_t> graph_t::add_node(std::shared_ptr<node_t> previous, tchecker::clock_rational_value_t delay)
{
  std::size_t id = _nodes.size();
  tchecker::clockval_t *raw = clockval_clone(*(previous->valuation()));
  auto new_valuation = std::shared_ptr<tchecker::clockval_t>(raw, &clockval_destruct_and_deallocate);

  add_delay(new_valuation, *new_valuation, delay);
  std::shared_ptr<node_t> result = std::make_shared<node_t>(previous->ta_state(), new_valuation, id, false, previous->flags().final());
  _nodes.emplace_back(result);
  return result;
}

std::shared_ptr<node_t> graph_t::add_node(tchecker::zg::zg_t::state_t & symb_state)
{
  std::size_t id = _nodes.size();
  // Now follows an ugly hack.
  // Due to the fact that zg states inherit from ta states (I believe this is a bad decision, but ...), 
  // we have to convert the state_sptr_t to a std::shared_ptr<tchecker::ta::state_t>. Uff...
  // To do so, we take the raw pointer to the chosen_symb and delete the destructor.
  std::shared_ptr<tchecker::ta::state_t> ta_ptr(symb_state.ptr(), [](tchecker::ta::state_t*){}); 
  std::shared_ptr<node_t> result = std::make_shared<node_t>(ta_ptr, symb_state->zone(), id, false, false);
  _nodes.emplace_back(result);
  return result;
}

void graph_t::add_action_edge(tchecker::zg::zg_t::transition_t & t, node_t & src, node_t & tgt)
{
  _edges.emplace_back(std::make_shared<action_edge_t>(t, src, tgt));
}

void graph_t::add_delay_edge(tchecker::clock_rational_value_t & delay, node_t & src, node_t & tgt)
{
  _edges.emplace_back(std::make_shared<delay_edge_t>(delay, src, tgt));
}

/* state_space_t */

state_space_t::state_space_t(std::shared_ptr<tchecker::ta::system_t const> system)
  : _concrete_sim_graph(system)
{
}

graph_t & state_space_t::graph()
{
  return _concrete_sim_graph;
}

void state_space_t::dot_output(std::ostream & os, std::string const & name)
{
  _concrete_sim_graph.dot_output(os, name);
}

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker