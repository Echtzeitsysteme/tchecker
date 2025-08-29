/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_EDGE_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_EDGE_HH

#include "tchecker/graph/edge.hh"
#include "tchecker/strong-timed-bisim/witness/witness_node.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

/*!
 \class edge_type_t
 \brief Type of edge of the witness graph of a strong timed bisimulation check
*/
class edge_type_t {
public:
  /*!
   \brief Constructor
   \param t1 : a transition of the first VCG
   \param t2 : a transition of the second VCG
   \param src : the source of this edge
   \param tgt : the target of this edge
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_type_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node_t> src,
              std::shared_ptr<node_t> tgt);

 /*!
  \brief Accessor
  \return shared pointer to the pair of edges
  */
  inline std::shared_ptr<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>> edge_pair_ptr() const { return _edge_pair; }

 /*!
  \brief Accessor
  \return the pair of edges
  */
  inline std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t> const & edge_pair() const { return *_edge_pair; }

 /*!
  \brief Accessor
  \return shared pointer to the source of this edge
  */
  inline std::shared_ptr<const node_t> src() const { return _src; }

 /*!
  \brief Accessor
  \return shared pointer to the target of this edge
  */
  inline std::shared_ptr<const node_t> tgt() const { return _tgt; }

 /*!
   \brief Less-than ordering on edges based on lexical ordering
   \param other : an edge
   \return true if lhs is less-than rhs w.r.t. the tuple of edges in e1 and e2
   \note _condition is not part of this comparison!
  */
  bool operator<(const edge_type_t& other) const;

  /*!
   \brief equal operator
   \param other : an edge type
   \return true if lhs is less-than rhs w.r.t. the tuple of edges in e1 and e2
   \note _condition is not part of this comparison!
  */
  bool operator==(const edge_type_t& other) const;

 private:
  std::shared_ptr<std::pair<tchecker::graph::edge_vedge_t, tchecker::graph::edge_vedge_t>> _edge_pair;
  std::shared_ptr<node_t> _src;
  std::shared_ptr<node_t> _tgt;
};

class edge_t {
 public:
  /*!
   \brief Constructor
   \param t1 : a transition of the first VCG
   \param t2 : a transition of the second VCG
   \param src : the source of this edge
   \param tgt : the target of this edge
   \param condition : the condition to use this edge
  */
  edge_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node_t> src,
         std::shared_ptr<node_t> tgt, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition);

  /*!
   \brief Constructor
   \param type : the type of the edge
   \param condition : the condition to use this edge
  */
  edge_t(std::shared_ptr<edge_type_t> type, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition);


  /*!
  \brief Accessor
  \return shared pointer to the type
  */
  std::shared_ptr<edge_type_t> type() const { return _type; }

  /*!
  \brief Accessor
  \return shared pointer to the condition
  */
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition() const { return _condition; }

  /*!
   \brief checks whether this edge is already represented by other
   \param other : an edge
   \return true if this has the same edge_pair, the same src, the same target and a condition that is at least as strong as that from other
  */
  bool is_subset(const edge_t& other) const;

 private:
  std::shared_ptr<edge_type_t> _type;
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> _condition;
};

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
