/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COMPARE_STATS_HH
#define TCHECKER_ALGORITHMS_COMPARE_STATS_HH

#include <unordered_set>

#include "tchecker/algorithms/stats.hh"
#include "tchecker/zg/state.hh"

#include "tchecker/strong-timed-bisim/witness/witness_graph.hh"

/*!
 \file stats.hh
 \brief Statistics for comparison algorithm
 */


namespace tchecker {

namespace strong_timed_bisim {

/*!
 \class stats_t
 \brief Statistics for comparison algorithm
 */
class stats_t : public tchecker::algorithms::stats_t {
public:
  /*!
  \brief Constructor
  */
  stats_t();

  /*!
   \brief Accessor
   \return A reference to the number of visited pair of states
  */
  long visited_pair_of_states() const;

  /*!
   \brief setter for the number of visited pair of states
   \post visited_pair_of_states is set to the given value
   */
  void set_visited_pair_of_states(long visited_pair_of_states);

  /*!
  \brief Accessor
  \return Reference to the answer whether the relationship is fulfilled
  */
  bool relationship_fulfilled() const;

  /*!
   \brief setter for relationship_fulfilled
   \post relationship_fulfilled is set to the given value
   */
  void set_relationship_fulfilled(bool relationship_fulfilled);

  /*!
   \brief Accessor
   \return Reference to the witness
  */
  std::shared_ptr<tchecker::strong_timed_bisim::witness::graph_t> witness() const;

  /*!
   \brief Init witness
   \post _witness is an empty graph
  */
  void init_witness(std::shared_ptr<tchecker::vcg::vcg_t> const & vcg1, 
                           std::shared_ptr<tchecker::vcg::vcg_t> const & vcg2);
  /*!
   \brief Extract statistics as attributes (key, value)
   \param m : attributes map
   \post every statistics has been added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private:
  long _visited_pair_of_states;  /*!< Number of visited pairs of states */
  bool _relationship_fulfilled;  /*!< Whether the relationship is fulfilled */
  std::shared_ptr<tchecker::strong_timed_bisim::witness::graph_t> _witness; /*!< If relationship_fulfilled, then it contains the witness */
};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COMPARE_STATS_HH
