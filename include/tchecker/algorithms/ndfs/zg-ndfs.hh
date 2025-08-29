/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_NDFS_ALGORITHM_HH
#define TCHECKER_ZG_NDFS_ALGORITHM_HH

#include <memory>
#include <ostream>
#include <string>
#include <tuple>

#include "tchecker/algorithms/ndfs/algorithm.hh"
#include "tchecker/algorithms/ndfs/graph.hh"
#include "tchecker/algorithms/ndfs/stats.hh"
#include "tchecker/graph/edge.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/ts/state_space.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/zg/path.hh"
#include "tchecker/zg/state.hh"
#include "tchecker/zg/transition.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace algorithms {

namespace zg_ndfs {

/*!
 \class node_t
 \brief Node of the liveness graph of a zone graph
 */
class node_t : public tchecker::algorithms::ndfs::node_t, public tchecker::graph::node_zg_state_t {
public:
  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
   */
  node_t(tchecker::zg::state_sptr_t const & s);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
   */
  node_t(tchecker::zg::const_state_sptr_t const & s);
};

/*!
\class node_hash_t
\brief Hash functor for nodes
*/
class node_hash_t {
public:
  /*!
  \brief Hash function
  \param n : a node
  \return hash value for n
  */
  std::size_t operator()(tchecker::algorithms::zg_ndfs::node_t const & n) const;
};

/*!
\class node_equal_to_t
\brief Equality check functor for nodes
*/
class node_equal_to_t {
public:
  /*!
  \brief Equality predicate
  \param n1 : a node
  \param n2 : a node
  \return true if n1 and n2 are equal (i.e. have same zone graph state), false otherwise
  */
  bool operator()(tchecker::algorithms::zg_ndfs::node_t const & n1, tchecker::algorithms::zg_ndfs::node_t const & n2) const;
};

/*!
 \class edge_t
 \brief Edge of the liveness graph of a zone graph
*/
class edge_t : public tchecker::graph::edge_vedge_t {
public:
  /*!
   \brief Constructor
   \param t : a zone graph transition
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_t(tchecker::zg::transition_t const & t);
};

/*!
 \class graph_t
 \brief Liveness graph over the zone graph
*/
class graph_t : public tchecker::graph::reachability::graph_t<
                    tchecker::algorithms::zg_ndfs::node_t, tchecker::algorithms::zg_ndfs::edge_t,
                    tchecker::algorithms::zg_ndfs::node_hash_t, tchecker::algorithms::zg_ndfs::node_equal_to_t> {
public:
  /*!
   \brief Constructor
   \param zg : zone graph
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a pointer on zg
   \note this graph keeps pointers to (part of) states and (part of) transitions allocated by zg. Hence, the graph
   must be destroyed *before* zg is destroyed, since all states and transitions allocated by zg are detroyed
   when zg is destroyed. See state_space_t below to store both fzg and this graph and destroy them in the expected
   order.
  */
  graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Destructor
  */
  virtual ~graph_t();

  using tchecker::graph::reachability::graph_t<tchecker::algorithms::zg_ndfs::node_t, tchecker::algorithms::zg_ndfs::edge_t,
                                               tchecker::algorithms::zg_ndfs::node_hash_t,
                                               tchecker::algorithms::zg_ndfs::node_equal_to_t>::attributes;

  /*!
   \brief Accessor
   \return underlying zone graph
  */
  inline tchecker::zg::zg_t & zg() const { return *_zg; }

  /*!
   \brief Accessor
   \return shared pointer to underlying zone graph
  */
  inline std::shared_ptr<tchecker::zg::zg_t> zg_ptr() const { return _zg; }

  /*!
   \brief Checks if an edge is an actual edge in the graph
   \return true (all edges are actual edges in the reachability graph)
   */
  bool is_actual_edge(edge_sptr_t const &) const { return true; }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  virtual void attributes(tchecker::algorithms::zg_ndfs::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  virtual void attributes(tchecker::algorithms::zg_ndfs::edge_t const & e, std::map<std::string, std::string> & m) const;

private:
  std::shared_ptr<tchecker::zg::zg_t> _zg; /*!< Zone graph */
};

/*!
 \brief Graph output
 \param os : output stream
 \param g : graph
 \param name : graph name
 \post graph g with name has been output to os
*/
std::ostream & dot_output(std::ostream & os, tchecker::algorithms::zg_ndfs::graph_t const & g, std::string const & name);

/*!
 \class state_space_t
 \brief State-space representation consisting of a zone graph and a reachability graph
 */
class state_space_t {
public:
  /*!
   \brief Constructor
   \param zg : zone graph
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a pointer on zg
   */
  state_space_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Accessor
   \return The zone graph
   */
  tchecker::zg::zg_t & zg();

  /*!
   \brief Accessor
   \return The reachability graph representing the state-space
   */
  tchecker::algorithms::zg_ndfs::graph_t & graph();

private:
  tchecker::ts::state_space_t<tchecker::zg::zg_t, tchecker::algorithms::zg_ndfs::graph_t>
      _ss; /*!< State-space representation */
};

namespace cex {

/*!
 \brief Type of symbolic counter-example
*/
using symbolic_cex_t = tchecker::zg::path::symbolic::lasso_path_t;

/*!
 \brief Compute a symbolic counter-example from a liveness graph of a zone graph
 \param g : liveness graph on a zone graph
 \return a lasso path from an initial node that visits a final node in g infinitely often if any,
 nullptr otherwise
 \note the returned pointer shall be deleted
*/
tchecker::algorithms::zg_ndfs::cex::symbolic_cex_t *
symbolic_counter_example(tchecker::algorithms::zg_ndfs::graph_t const & g);

/*!
 \brief Symbolic counter-example output
 \param os : output stream
 \param cex : counter example
 \param name : counter example name
 \post cex has been output to os
 \return os after output
 */
std::ostream & dot_output(std::ostream & os, tchecker::algorithms::zg_ndfs::cex::symbolic_cex_t const & cex,
                          std::string const & name);

} // namespace cex

/*!
 \class algorithm_t
 \brief Nested DFS algorithm over the zone graph
*/
class algorithm_t
    : public tchecker::algorithms::ndfs::algorithm_t<tchecker::zg::zg_t, tchecker::algorithms::zg_ndfs::graph_t> {
public:
  using tchecker::algorithms::ndfs::algorithm_t<tchecker::zg::zg_t, tchecker::algorithms::zg_ndfs::graph_t>::algorithm_t;
};

/*!
 \brief Run nested DFS algorithm on the zone graph of a system
 \param sysdecl : system declaration
 \param labels : comma-separated string of labels
 \param block_size : number of elements allocated in one block
 \param table_size : size of hash tables
 \pre labels must appear as node attributes in sysdecl
 \return statistics on the run and the liveness graph
 \throw std::runtime_error : if clock bounds cannot be computed for the system modeled by sysdecl
 */
std::tuple<tchecker::algorithms::ndfs::stats_t, std::shared_ptr<tchecker::algorithms::zg_ndfs::state_space_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels = "", std::size_t block_size = 10000,
    std::size_t table_size = 65536);

} // namespace zg_ndfs

} // namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ZG_NDFS_ALGORITHM_HH
