/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH
#define TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH

#include "tchecker/strong-timed-bisim/stats.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace strong_timed_bisim {

/*
 \ class Lieb_et_al
 \brief Implements the on-the-fly algorithm to check strong timed bisimulation of Lieb et al.
 */
class Lieb_et_al {

  public:

  /*!
   \brief running the algorithm of Lieb et al. (TODO: add paper ref here)
   \param input_first : the first vcg
   \param input_second : the second vcg
   */
  tchecker::strong_timed_bisim::stats_t run(std::shared_ptr<tchecker::zg::zg_t const> input_first, std::shared_ptr<tchecker::zg::zg_t const> input_second);

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
