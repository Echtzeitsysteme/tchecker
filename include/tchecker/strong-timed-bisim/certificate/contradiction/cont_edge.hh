/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_CONT_EDGE_HH
#define TCHECKER_STRONG_TIMED_BISIM_CONTRADICTION_CONT_EDGE_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_edge.hh"
#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_node.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

using edge_t = tchecker::strong_timed_bisim::certificate::edge_t<node_t>;

class delay_edge_t {

public:
  /*!
   \brief Constructor
   \param delay : the amount of time passed
   \param source : the source of this edge
   \param target : the target of this edge
   \post this node keeps a shared pointer on the vedge in t
  */
  delay_edge_t(clock_rational_value_t delay, std::shared_ptr<node_t> source, std::shared_ptr<node_t> target);

  /*!
   \brief Accessor
   \return the delay
   */
  inline clock_rational_value_t delay() const {return _delay; };

  /*!
  \brief Accessor
  \return shared pointer to the source of this edge
  */
  inline std::shared_ptr<node_t> src() const { return _src; }

  /*!
   \brief Accessor
   \return shared pointer to the target of this edge
   */
  inline std::shared_ptr<node_t> tgt() const { return _tgt; }

  /*!
  \brief Accessor to node attributes
  \param m : the map to add the attributes
  \post The attributes of this node are added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private: 
  clock_rational_value_t _delay;
  std::shared_ptr<node_t> _src;
  std::shared_ptr<node_t> _tgt;
};

} // end of namespace contra

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif