/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CONCUR19_ALGORITHM_HH
#define TCHECKER_CONCUR19_ALGORITHM_HH

#include <memory>
#include <string>

#include "tchecker/algorithms/covreach/algorithm.hh"
#include "tchecker/algorithms/covreach/stats.hh"
#include "tchecker/clockbounds/cache.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/clockbounds/solver.hh"
#include "tchecker/graph/edge.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/graph/subsumption_graph.hh"
#include "tchecker/refzg/path.hh"
#include "tchecker/refzg/refzg.hh"
#include "tchecker/refzg/state.hh"
#include "tchecker/refzg/transition.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ts/state_space.hh"
#include "tchecker/waiting/waiting.hh"

/*!
 \file concur19.hh
 \brief Covering reachability algorithm over the local-time zone graph using
 sync-subsumption
 (see: R. Govind, Frédéric Herbreteau, B. Srivathsan, Igor Walukiewicz:
 "Revisiting Local Time Semantics for Networks of Timed Automata". CONCUR 2019:
 16:1-16:15)
*/

namespace tchecker {

namespace algorithms {

namespace concur19 {

/*!
 \class node_t
 \brief Node of the subsumption graph over the local-time zone graph
 */
class node_t : public tchecker::waiting::element_t,
               public tchecker::graph::node_flags_t,
               public tchecker::graph::node_refzg_state_t {
public:
  /*!
   \brief Constructor
   \param s : a state of the local-time zone graph
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::refzg::state_sptr_t const & s, bool initial = false, bool final = false);

  /*!
   \brief Constructor
   \param s : a state of the local-time zone graph
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::refzg::const_state_sptr_t const & s, bool initial = false, bool final = false);
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
  \return hash value for n based on the discrete part of n (i.e. the tuple of
  locations and integer variable valuations) since we need to cover nodes with
  same discrete part
  */
  std::size_t operator()(tchecker::algorithms::concur19::node_t const & n) const;
};

/*!
\class node_le_t
\brief Covering predicate for nodes
*/
class node_le_t {
public:
  /*!
  \brief Constructor
  \param local_lu : local LU clock bounds
  \param table_size : size of clock bounds cache
  \post this keeps a shared pointer to local_lu
  \throw std::invalid_argument : if local_lu points to nullptr
  */
  node_le_t(std::shared_ptr<tchecker::clockbounds::local_lu_map_t> const & local_lu, std::size_t table_size);

  /*!
  \brief Covering predicate for nodes
  \param n1 : a node
  \param n2 : a node
  \return true if n1 and n2 have same discrete part and the zone of n1 is
  sync-aLU subsumed in the zone of n2, false otherwise
  */
  bool operator()(tchecker::algorithms::concur19::node_t const & n1, tchecker::algorithms::concur19::node_t const & n2) const;

private:
  using const_vloc_sptr_hash_t = tchecker::intrusive_shared_ptr_hash_t;
  using const_vloc_sptr_equal_t = std::equal_to<tchecker::const_vloc_sptr_t>;

  mutable tchecker::clockbounds::bounded_cache_local_lu_map_t<const_vloc_sptr_hash_t, const_vloc_sptr_equal_t>
      _cached_local_lu; /*!< Cached local LU clock bounds*/
};

/*!
 \class edge_t
 \brief Edge of the subsumption graph of a local-time zone graph
*/
class edge_t : public tchecker::graph::edge_vedge_t {
public:
  /*!
   \brief Constructor
   \param t : a zone graph transition
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_t(tchecker::refzg::transition_t const & t);
};

/*!
 \class graph_t
 \brief Subsumption graph over the local-time zone graph
*/
class graph_t
    : public tchecker::graph::subsumption::graph_t<tchecker::algorithms::concur19::node_t, tchecker::algorithms::concur19::edge_t,
                                                   tchecker::algorithms::concur19::node_hash_t,
                                                   tchecker::algorithms::concur19::node_le_t> {
public:
  /*!
   \brief Constructor
   \param refzg : zone graph with reference clocks
   \param local_lu : local LU bounds map for aLU covering
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a shared pointer on refzg and on local_lu
   \note this graph keeps pointers to (part of) states and (part of) transitions allocated by refzg. Hence, the graph
   must be destroyed *before* refzg is destroyed, since all states and transitions allocated by refzg are detroyed
   when refzg is destroyed. See state_space_t below to store both refzg and this graph and destroy them in the expected
   order.
  */
  graph_t(std::shared_ptr<tchecker::refzg::refzg_t> const & refzg,
          std::shared_ptr<tchecker::clockbounds::local_lu_map_t> const & local_lu, std::size_t block_size,
          std::size_t table_size);

  /*!
   \brief Accessor
   \return Shared pointer to underlying zone graph with reference clocks
  */
  inline std::shared_ptr<tchecker::refzg::refzg_t> const & refzg_ptr() const { return _refzg; }

  /*!
   \brief Accessor
   \return Underlying zone graph with reference clocks
  */
  inline tchecker::refzg::refzg_t const & refzg() const { return *_refzg; }

  using tchecker::graph::subsumption::graph_t<tchecker::algorithms::concur19::node_t, tchecker::algorithms::concur19::edge_t,
                                              tchecker::algorithms::concur19::node_hash_t,
                                              tchecker::algorithms::concur19::node_le_t>::attributes;

  /*!
   \brief Checks is an edge is an actual edge (not a subsumption edge)
   \param e : an edge
   \return true if e is an actual edge, false otherwise (e is a subsumption edge)
   */
  bool is_actual_edge(edge_sptr_t const & e) const;

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  virtual void attributes(tchecker::algorithms::concur19::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  virtual void attributes(tchecker::algorithms::concur19::edge_t const & e, std::map<std::string, std::string> & m) const;

private:
  std::shared_ptr<tchecker::refzg::refzg_t> _refzg; /*!< Zone graph with reference clocks */
};

/*!
 \brief Graph output
 \param os : output stream
 \param g : graph
 \param name : graph name
 \post graph g with name has been output to os
*/
std::ostream & dot_output(std::ostream & os, tchecker::algorithms::concur19::graph_t const & g, std::string const & name);

/*!
 \class state_space_t
 \brief State-space representation consisting of a zone graph with reference clocks and a subsumption graph
 */
class state_space_t {
public:
  /*!
   \brief Constructor
   \param refzg : zone graph with reference clocks
   \param local_lu : local LU bounds map for aLU covering
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a pointer on zg
   */
  state_space_t(std::shared_ptr<tchecker::refzg::refzg_t> const & refzg,
                std::shared_ptr<tchecker::clockbounds::local_lu_map_t> const & local_lu, std::size_t block_size,
                std::size_t table_size);

  /*!
   \brief Accessor
   \return The zone graph with reference clocks
   */
  tchecker::refzg::refzg_t & refzg();

  /*!
   \brief Accessor
   \return The subsumption graph representing the state-space
   */
  tchecker::algorithms::concur19::graph_t & graph();

private:
  tchecker::ts::state_space_t<tchecker::refzg::refzg_t, tchecker::algorithms::concur19::graph_t>
      _ss; /*!< State-space representation */
};

namespace cex {

namespace symbolic {

/*!
 \brief Type of symbolic reachability counter-example
*/
using cex_t = tchecker::refzg::path::finite_path_t;

/*!
 \brief Compute a counter-example from a covering reachability graph of a zone graph with
 reference clocks
 \param g : subsumption graph on a zone graph with reference clocks
 \return a finite path from an initial node to a final node in g if any, nullptr otherwise
 \note the returned pointer shall be deleted
*/
tchecker::algorithms::concur19::cex::symbolic::cex_t * counter_example(tchecker::algorithms::concur19::graph_t const & g);

/*!
 \brief Counter-example output
 \param os : output stream
 \param cex : counter example
 \param name : counter example name
 \post cex has been output to os
 \return os after output
 */
std::ostream & dot_output(std::ostream & os, tchecker::algorithms::concur19::cex::symbolic::cex_t const & cex,
                          std::string const & name);

} // namespace symbolic

} // namespace cex

/*!
 \class algorithm_t
 \brief Covering reachability algorithm over the local-time zone graph
*/
class algorithm_t
    : public tchecker::algorithms::covreach::algorithm_t<tchecker::refzg::refzg_t, tchecker::algorithms::concur19::graph_t> {
public:
  using tchecker::algorithms::covreach::algorithm_t<tchecker::refzg::refzg_t,
                                                    tchecker::algorithms::concur19::graph_t>::algorithm_t;
};

/*!
 \brief Run covering reachability algorithm on the local-time zone graph of a
 system
 \param sysdecl : system declaration
 \param labels : comma-separated string of labels
 \param search_order : search order
 \param covering : covering policy
 \param block_size : number of elements allocated in one block
 \param table_size : size of hash tables
 \pre labels must appear as node attributes in sysdecl
 search_order must be either "dfs" or "bfs"
 \return statistics on the run and a representation of the state-space as a subsumption graph
 \throw std::runtime_error : if clock bounds cannot be computed for the system modeled as sysdecl
 */
std::tuple<tchecker::algorithms::covreach::stats_t, std::shared_ptr<tchecker::algorithms::concur19::state_space_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels = "",
    std::string const & search_order = "bfs",
    tchecker::algorithms::covreach::covering_t covering = tchecker::algorithms::covreach::COVERING_FULL,
    std::size_t block_size = 10000, std::size_t table_size = 65536);

} // end of namespace concur19

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_CONCUR19_ALGORITHM_HH
