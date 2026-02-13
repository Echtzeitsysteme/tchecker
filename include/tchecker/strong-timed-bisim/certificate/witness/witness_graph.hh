/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_GRAPH_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_GRAPH_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_graph.hh"

#include "tchecker/strong-timed-bisim/certificate/witness/witness_node.hh"
#include "tchecker/strong-timed-bisim/certificate/witness/witness_edge.hh"

#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/strong-timed-bisim/visited_map.hh"


namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

using base_graph_t = tchecker::strong_timed_bisim::certificate::graph_t<node_t, edge_t>;

/*!
 \class graph_t
 \brief Graph of the witness of a strong timed bisimulation check
*/
class graph_t : public base_graph_t {
public:

  /*!
   \brief Constructor
   \param vcg1 : first vcg
   \param vcg1 : second vcg
   \note this keeps a pointer on the vcg
  */
  graph_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2);

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
   \brief Graph output
   \param os : output stream
   \param name : graph name
   \post graph with name has been output to os
  */
  std::ostream & dot_output(std::ostream & os, std::string const & name) override;

protected:

  /*!
   \brief removes redundant edges
  */
  void edge_cleanup();

  /*!
   \brief removes unused nodes
  */
  void node_cleanup();

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

  void add_node_that_already_exists(std::shared_ptr<node_t> to_add);

  /*!
   \brief returns a set that contains all types of edges, i.e. all available combinations of edge_pair, src, and target.
   \return a set of all edge types available in _edges
   */
  std::shared_ptr<std::set<edge_t>> get_edges();

  tchecker::zg::state_sptr_t create_symbolic_state(tchecker::ta::state_t &ta_state, 
                                                   tchecker::virtual_constraint::virtual_constraint_t &vc,
                                                   tchecker::zg::state_sptr_t &init,
                                                   bool first_not_second);
};

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
