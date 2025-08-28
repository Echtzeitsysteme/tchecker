/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_GRAPH_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_GRAPH_HH

#include "tchecker/strong-timed-bisim/witness/witness_node.hh"
#include "tchecker/strong-timed-bisim/witness/witness_edge.hh"

#include "tchecker/vcg/vcg.hh"
#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/strong-timed-bisim/visited_map.hh"


namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

/*!
 \class graph_t
 \brief Witness Graph over VCG
*/
class graph_t {
public:

  /*!
   \brief Constructor
   \param vcg1 : first vcg
   \param vcg1 : second vcg
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a pointer on the vcg
  */
  graph_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2);

  /*!
   \brief Accessor
   \return pointer to first vcg
  */
  std::shared_ptr<tchecker::vcg::vcg_t> vcg1_ptr() { return _vcg1; }

  /*!
   \brief Accessor
   \return first vcg
  */
  tchecker::vcg::vcg_t const & vcg1() const { return *_vcg1; }

  /*!
   \brief Accessor
   \return pointer to the second vcg
  */
  std::shared_ptr<tchecker::vcg::vcg_t> vcg2_ptr() { return _vcg2; }

  /*!
   \brief Accessor
   \return second vcg
  */
  tchecker::vcg::vcg_t const & vcg2() const { return *_vcg2; }

  /*!
   \brief Accessor
   \return the vector of all contained nodes
  */
  std::shared_ptr<std::vector<std::shared_ptr<node_t>>> nodes() const { return _nodes; }

  /*!
   \brief Add a node
   \param A_node : the node of the first vcg
   \param B_node : the node of the second vcg
   \param initial : whether this node is the initial node
   \post the new node was added to the graph
   \note In case there already exists a node with these locations, initial is not used
  */
  void add_node(tchecker::zg::state_sptr_t A_node, tchecker::zg::state_sptr_t B_node, bool initial);

 /*!
   \brief Accessor
   \return the vector of all contained edges
  */
  std::shared_ptr<std::vector<std::shared_ptr<edge_t>>> edges() const { return _edges; }

  /*!
   \brief Add an edge
   \param A_source : the source node of the first vcg
   \param A_target : the target node of the first vcg
   \param A_transition : the transition of the first vcg
   \param B_source : the source node of the second vcg
   \param B_target : the target node of the second vcg
   \param B_transition : the transition of the second vcg
   \post the new transition is added to _edges
  */
  void add_edge(tchecker::zg::state_sptr_t A_source, tchecker::ta::state_t &A_target, tchecker::zg::transition_t & A_transition, 
                tchecker::zg::state_sptr_t B_source, tchecker::ta::state_t &B_target, tchecker::zg::transition_t & B_transition,
                std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition);

  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  void attributes(tchecker::strong_timed_bisim::witness::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  void attributes(tchecker::strong_timed_bisim::witness::edge_t const & e, std::map<std::string, std::string> & m) const;

  /*!
   \brief Creates a witness graph from a given visited_map in case the given NTA are timed bisimilar
   \param visited : the visited map
   \param first : the initial symbolic state of the first vcg
   \param second : the initial symbolic state of the second vcg
   \post The graph contains a valid witness
  */
  void create_witness_from_visited(tchecker::strong_timed_bisim::visited_map_t &visited, 
                                   tchecker::zg::state_sptr_t first_init, tchecker::zg::state_sptr_t second_init);

  /*!
   \brief removes redundant edges
  */
  void edge_cleanup();


private:

  void add_node(tchecker::ta::state_t &first, tchecker::ta::state_t &second,
                tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> &vcs);

  void add_node(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> &loc_pair,
                tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> &vcs);

  tchecker::zg::state_sptr_t create_symbolic_state(tchecker::ta::state_t &ta_state, 
                                                   tchecker::virtual_constraint::virtual_constraint_t &vc,
                                                   tchecker::zg::state_sptr_t &init,
                                                   bool first_not_second);

  /*!
   \brief returns the node corresponding to this pair of symb. states. Nullptr in case there exists none.
  */
  std::shared_ptr<node_t> find_node(tchecker::zg::state_sptr_t A_node, tchecker::zg::state_sptr_t B_node);

  /*!
   \brief returns the node corresponding to this pair of symb. states. Nullptr in case there exists none.
  */
  std::shared_ptr<node_t> find_node(tchecker::ta::state_t &first, tchecker::ta::state_t &second);

  /*!
   \brief returns a set that contains all types of edges, i.e. all available combinations of edge_pair, src, and target.
   \return a set of all edge types available in _edges
   */
  std::shared_ptr<std::set<edge_type_t>> find_all_edge_types();

  const std::shared_ptr<tchecker::vcg::vcg_t> _vcg1;
  const std::shared_ptr<tchecker::vcg::vcg_t> _vcg2;

  std::shared_ptr<std::vector<std::shared_ptr<node_t>>> _nodes;
  std::shared_ptr<std::vector<std::shared_ptr<edge_t>>> _edges;

  // any added nodes gets an unique id. This is the corresponding counter.
  std::size_t _nodes_id_counter;
};

/*!
 \brief Graph output
 \param os : output stream
 \param g : graph
 \param name : graph name
 \post graph g with name has been output to os
*/
std::ostream & dot_output(std::ostream & os, graph_t & g, std::string const & name);

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
