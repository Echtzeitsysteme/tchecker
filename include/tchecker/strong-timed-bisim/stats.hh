/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COMPARE_STATS_HH
#define TCHECKER_ALGORITHMS_COMPARE_STATS_HH

#include "tchecker/algorithms/stats.hh"

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
  unsigned long visited_pair_of_states() const;

  /*!
  \brief Accessor
  \return Number of visited transitions
  */
  unsigned long visited_transitions() const;

  /*!
  \brief Accessor
  \return The deepest path checked
  */

  unsigned long deepest_path() const;

  /*!
  \brief Accessor
  \return Reference to the answer whether the relationship is fulfilled
  */
  bool relationship_fulfilled() const;

  /*!
   \brief Extract statistics as attributes (key, value)
   \param m : attributes map
   \post every statistics has been added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private:
  unsigned long _visited_pair_of_states;      /*!< Number of visited pairs of states */
  unsigned long _visited_transitions; /*!< Number of visited transitions */
  unsigned long _deepest_path; /*!< Number of visited transitions */
  bool _relationship_fulfilled;  /*< Whether the relationship is fulfilled */
};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COMPARE_STATS_HH
