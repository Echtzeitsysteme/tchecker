/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_SYMBOLIC_SYMBOLIC_GRAPH_HH
#define TCHECKER_TCK_SIMULATE_SYMBOLIC_SYMBOLIC_GRAPH_HH

#include "tchecker/simulate/simulate.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/ta/ta.hh"


/*!
 \file concrete_graph.hh
 \brief Concrete simulation graph
*/

namespace tchecker {

namespace simulate {

namespace concrete {

  /*!
\class node_t
\brief Nodes of simulation graph
*/
class node_t {
public:
  /*!
   \brief Constructor
   \param ta_state : a ta state
   \param valuation : a clock valuation
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(std::shared_ptr<tchecker::ta::state_t> ta_state, tchecker::clockval_t *valuation, size_t id,
         bool initial = false, bool final = false);

  /*!
   \brief Copy Constructor
   \param other : the node to copy
   */
  node_t(node_t const & other);

  /*!
   \brief Getter of the attributes
   \param system : TA
   \param m : map to store the attributes
   \post attributes of this is stored to m
   */
  void attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const;

  /*!
   \brief Getter for the flags
   \return the flags
  */
  tchecker::graph::node_flags_t flags() const;
 
  /*!
   \brief Getter for the id
   \return the id
   */
   std::size_t id() const;

  /*!
   \brief Setter for the id
   \param id : the new id
   \post _id is set to id
   */
  void set_id(std::size_t id);

  /*!
   \brief Getter for the clockval
   \return the valuation
   */
  tchecker::clockval_t * valuation() const;

  /*!
   \brief Getter for the TA State
   \return THe ta_state
   */
  std::shared_ptr<tchecker::ta::state_t> ta_state() const;

 private: 
  tchecker::graph::node_flags_t _flags;
  std::shared_ptr<tchecker::ta::state_t> _ta_state;
  tchecker::clockval_t * _valuation;
  std::size_t _id;
};

/*!
 \class edge_t
 \brief Edge of the concrete simulation graph
*/
class edge_t {
public:
  /*!
   \brief Constructor
   \param src : the src
   \param tgt : the target
  */
  edge_t(node_t & src, node_t & tgt);

  /*!
   \brief Accessor to edge attributes
   \param m : a map (key, value) of attributes
   \post attributes of this have been added to map m
  */
  virtual void attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor of _src
   \return _src
   */
  node_t src();
  
  /*!
   \brief Accessor of _tgt
   \return _tgt
   */
  node_t tgt();

 private:
  node_t _src;
  node_t _tgt;
};

/*!
 \class action_edge_t
 \brief Action Edge of the concrete simulation graph
*/
class action_edge_t : public edge_t {
public:
  /*!
   \brief Constructor
   \param t : a ta transition
  */
  action_edge_t(tchecker::zg::zg_t::transition_t & t, node_t & src, node_t & tgt);

  /*!
   \brief Accessor to edge attributes
   \param m : a map (key, value) of attributes
   \post attributes of this have been added to map m
  */
  void attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const override;

 private:
  tchecker::zg::zg_t::transition_t & _ta_trans;
};

/*!
 \class delay_edge_t
 \brief Delay edge of the concrete simulation graph
*/
class delay_edge_t : public edge_t {
public:
  /*!
   \brief Constructor
   \param t : a ta transition
  */
  delay_edge_t(tchecker::clock_rational_value_t & delay, node_t & src, node_t & tgt);

  /*!
   \brief Accessor to edge attributes
   \param m : a map (key, value) of attributes
   \post attributes of this have been added to map m
  */
  void attributes(tchecker::ta::system_t const & system, std::map<std::string, std::string> & m) const override;

 private:
  tchecker::clock_rational_value_t _delay;
};



/*!
 \class graph_t
 \brief Concrete simulation graph
*/
class graph_t {
public:
  /*!
   \brief Constructor
  */
  graph_t(std::shared_ptr<tchecker::ta::system_t const> system);

  /*!
   \brief adds a new node to the graph
   \param ta_state : the ta_state of the new node
   \param valuation : the valuation of the new node
   \param initial : whether this node is an initial node
   \param final : whether this node is a final node
   \return a shared ptr on the newly added node
   \post node is added to nodes
   */
  std::shared_ptr<node_t> add_node(std::shared_ptr<tchecker::ta::state_t> ta_state, tchecker::clockval_t * valuation,
                       bool initial = false, bool final = false);
  
  /*!
   \brief adds a new node to the graph
   \param previous : the previous node
   \param delay : the delay to add to the previous ndoe
   \return a shared ptr on the newly added node
   \post node is added to nodes
   */
  std::shared_ptr<node_t> add_node(std::shared_ptr<node_t> previous, tchecker::clock_rational_value_t delay);

  /*!
   \brief adds a new node to the graph
   \param previous : the previous node
   \param new_symb_state : the symbolic state of the new node
   \return a shared ptr on the newly added node
   \post node is added to nodes
  */
  std::shared_ptr<node_t> add_node(tchecker::zg::zg_t::state_t & symb_state);

  /*!
   \brief adds a new action edge to the graph
   \param t : the corresponding ta_transition
   \param src : the src node
   \param tgt : the tgt node
   \post edge is added to edges
   */
  void add_action_edge(tchecker::zg::zg_t::transition_t & t, node_t & src, node_t & tgt);

  /*!
   \brief adds a new delay edge to the graph
   \param delay : the delay
   \param src : the src node
   \param tgt : the tgt node
   \post edge is added to edges
   */
  void add_delay_edge(tchecker::clock_rational_value_t & delay, node_t & src, node_t & tgt);

  /*!
   \brief Graph output
   \param os : output stream
   \param g : graph
   \param name : graph name
   \post graph g with name has been output to os
  */
  void dot_output(std::ostream & os, std::string const & name);

private:
  std::shared_ptr<tchecker::ta::system_t const> _system;
  std::vector<std::shared_ptr<node_t>> _nodes;
  std::vector<std::shared_ptr<edge_t>> _edges;
};

/*!
 \class state_space_t
 \brief State-space representation consisting of a zone graph and a reachability multi-graph
 */
class state_space_t : public tchecker::simulate::state_space_t{
public:

  state_space_t(std::shared_ptr<tchecker::ta::system_t const> system);

  /*!
   \brief Accessor
   \return The reachability graph representing the state-space
   */
  graph_t & graph();

  /*!
   \brief Graph output
   \param os : output stream
   \param name : graph name
   \post graph g with name has been output to os
  */
  void dot_output(std::ostream & os, std::string const & name) override;

private:
  graph_t _concrete_sim_graph;
};


} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker

#endif