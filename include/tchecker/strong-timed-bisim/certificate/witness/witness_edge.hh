/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_EDGE_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_EDGE_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_edge.hh"
#include "tchecker/strong-timed-bisim/certificate/witness/witness_node.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

using base_edge_t = tchecker::strong_timed_bisim::certificate::edge_t<node_t>;

/*!
 \class edge_t
 \brief Edge of the witness of a strong timed bisimulation check
*/
class edge_t : public base_edge_t {
 public:
  /*!
   \brief Constructor
   \param t1 : a transition of the first VCG
   \param t2 : a transition of the second VCG
   \param src : the source of this edge
   \param tgt : the target of this edge
   \param condition : the condition to use this edge
  */
  edge_t(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2, std::shared_ptr<node_t> source,
         std::shared_ptr<node_t> target, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition);

  /*!
   \brief Constructor
   \param type : the type of the edge
   \param condition : the condition to use this edge
  */
  edge_t(std::shared_ptr<base_edge_t> type, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> condition);

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
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> _condition;
};

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
