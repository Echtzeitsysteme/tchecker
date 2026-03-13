/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_CONT_DAG_HH
#define TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_CONT_DAG_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_graph.hh"

#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_edge.hh"
#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_node.hh"

#include "tchecker/vcg/vcg.hh"
#include "tchecker/strong-timed-bisim/non_bisim_cache.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

using base_graph_t = tchecker::strong_timed_bisim::certificate::graph_t<node_t, edge_t>;


/*!
 \class cont_dag_t
 \brief the directed, acyclic graph of the contradiction
 */
class cont_dag_t : public base_graph_t {
public:
  /*!
   \brief Constructor
   \param vcg1 : first vcg
   \param vcg1 : second vcg
   \param first_init : the first ta state
   \param second_init : the second ta state
   \param first_invariant : the invariant of first_init
   \param second_invariant : the invariant of second_init
   \param max_delay : the maximum allowed delay
   \param previous : the dag, this is a subdag of. Nullptr if this is the init.
   \note this keeps a pointer on the vcg
  */
  cont_dag_t(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2, 
             tchecker::ta::state_t & first_init, tchecker::ta::state_t & second_init,
             tchecker::clockval_t & first_clockval, tchecker::clockval_t & second_clockval,
             tchecker::clock_constraint_container_t & first_invariant, tchecker::clock_constraint_container_t & second_invariant,
             std::size_t max_delay, tchecker::strong_timed_bisim::contra::cont_dag_t *previous);

  cont_dag_t(const cont_dag_t & other) = delete;

  /*!
   \brief Creates a contradiction DAG from a given non_bisim_cache in case the given NTA are not timed bisimilar
   \param non_bisim_cache : the cache of the virtual clock algorithm
   \param src : the initial node of this dag. Nullptr if _root shall be used.
   \return whether the creation of the CDAG was successfull
   \post If the return value is true, the graph contains a valid Contradiction DAG
   */
  bool create_cont_from_non_bisim_cache(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, 
                                        std::shared_ptr<node_t> src = nullptr);

  /*!
   \brief Accessor
   \return the vector of all contained delays
  */
  std::shared_ptr<std::vector<std::shared_ptr<delay_edge_t>>> delays() const { return _delays; }

  /*!
   \brief Add a delay
   \param delay : the delay to add
  */
  std::shared_ptr<delay_edge_t> add_delay(std::shared_ptr<delay_edge_t> delay);

  /*!
   \brief Add a delay
   \param args : the arguments for the constructor of the delay to add
  */
  template <typename... Args>
  std::shared_ptr<delay_edge_t> add_delay(Args&&... args)
  {
    auto to_add = std::make_shared<delay_edge_t>(std::forward<Args>(args)...);
    add_delay(to_add);
    return to_add;
  }

  /*!
   \brief Graph output
   \param os : output stream
   \param name : graph name
   \post graph with name has been output to os
  */
  std::ostream & dot_output(std::ostream & os, std::string const & name) override;

protected:

  void add_node_that_already_exists(std::shared_ptr<node_t> to_add) override;

private:

  /*!
   \brief If a synchronized node cannot be a leaf and does not have a helpful delay, this method finds the next action transition to use and adds it to the DAG
   \param non_bisim_cache : the non_bisim_cache
   \param src : the src of the action transition to find
   \return whether the search was successfull
   */
  bool add_non_bisim_action_transition(tchecker::strong_timed_bisim::non_bisim_cache_t &non_bisim_cache, std::shared_ptr<node_t> src);


  /*!
   \brief adds nodes and edges of other
   \param other : the other dag
   \param src : where other shall be appended
   \param trans_1 : where the first vedge between src and the initial of other comes from
   \param trans_2 : where the second vedge between src and the initial of other comes from
   \post all nodes and edges of other are added to this
   */
  void add_nodes_and_edges_of(cont_dag_t & other, std::shared_ptr<node_t> src, tchecker::zg::zg_t::sst_t trans_1, tchecker::zg::zg_t::sst_t trans_2);

  /*!
   \brief Checks whether a node is element of this or any upper CDAG
   \param node : the node to check
   \return whether the node is already element of this or any upper CDAG
   */
  bool does_node_exist_in_this_or_upper(std::shared_ptr<node_t> node);

  std::shared_ptr<node_t> _root;

  const std::shared_ptr<std::vector<std::shared_ptr<delay_edge_t>>> _delays;
  const std::size_t _max_delay;
  const bool _urgent_clk_exists;
  tchecker::strong_timed_bisim::contra::cont_dag_t *_previous;
};

} // end of namespace contra

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif