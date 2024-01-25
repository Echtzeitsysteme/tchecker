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
  std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>::size_type visited_pair_of_states() const;

  /*!
   \brief setter for the number of visited pair of states
   \post visited_pair_of_states is set to the given value
   */
  void set_visited_pair_of_states(std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>::size_type visited_pair_of_states);

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
   \brief Extract statistics as attributes (key, value)
   \param m : attributes map
   \post every statistics has been added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private:
  std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>::size_type _visited_pair_of_states;      /*!< Number of visited pairs of states */
  bool _relationship_fulfilled;  /*< Whether the relationship is fulfilled */
};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COMPARE_STATS_HH
