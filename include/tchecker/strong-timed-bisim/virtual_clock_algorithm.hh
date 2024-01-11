/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH
#define TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH

#include "tchecker/strong-timed-bisim/stats.hh"
#include "tchecker/vcg/vcg.hh"
#include "tchecker/vcg/state.hh"
#include "tchecker/utils/zone_container.hh"

namespace tchecker {

namespace strong_timed_bisim {

/*
 \ class Lieb_et_al
 \brief Implements the on-the-fly algorithm to check strong timed bisimulation of Lieb et al.
 */
class Lieb_et_al {

public:

  Lieb_et_al() = delete;

  /*!
  \brief Constructor
  \param input_first : the vcg of the first TA
  \param input_second : the vcg of the second TA
   */
  Lieb_et_al(std::shared_ptr<tchecker::vcg::vcg_t> input_first, std::shared_ptr<tchecker::vcg::vcg_t> input_second);

  /*!
   \brief running the algorithm of Lieb et al. (TODO: add paper ref here)
   \param input_first : the first vcg
   \param input_second : the second vcg
   */
  tchecker::strong_timed_bisim::stats_t run();

private:

  /*!
   \brief check-for-outgoing-transitions-impl function of Lieb et al.
   \param symb_state_A : the symbolic state that belongs to the first vcg
   \param symb_state_B : the symbolic state that belongs to the second vcg
   \param A_notB : a boolean that indicates which symbolic state should be checked
   \param func : a function that takes two symbolic states and returns a list of virtual constraint
   \return a list of virtual constraints that cannot be simulated.
   \note the result is allocated at the heap and must be freed.
   */
std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
check_for_outgoing_transitions( std::shared_ptr<tchecker::vcg::state_t const> symb_state_A,
                                std::shared_ptr<tchecker::vcg::state_t const> symb_state_B,
                                bool A_notB,
                                std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
                                  (*func)(std::shared_ptr<tchecker::vcg::state_t>, std::shared_ptr<tchecker::vcg::state_t>));

  const std::shared_ptr<tchecker::vcg::vcg_t> _A;
  const std::shared_ptr<tchecker::vcg::vcg_t> _B;

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
