/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH
#define TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH

#define TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED 0xDEADBEEF

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/strong-timed-bisim/stats.hh"
#include "tchecker/vcg/vcg.hh"
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

  struct custom_hash {
    size_t operator()(const std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> &to_hash) const {
      std::size_t h1 = tchecker::zg::hash_value(*(to_hash.first));
      std::size_t h2 = tchecker::zg::hash_value(*(to_hash.second));

      // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
      std::size_t h = (h1 + 0x9e3779b9 + (TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED <<6) + (TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED>>2));
      h ^= (h2 + 0x9e3779b9 + + (TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED <<6) + (TCHECKER_STRONG_TIMED_BISIM_VIRTUAL_CLOCK_ALGORITHM_HH_SEED>>2));
      return h;
    }
  };

  struct custom_equal {
    bool operator() (const std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> &p1, const std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t> &p2) const {
      return (*(p1.first) == *(p2.first)) && (*(p1.second) == *(p2.second));
    }
  };

  /*!
   \brief check-for-virt-bisim function of Lieb et al.
   \param symb_state_first : the symbolic state that belongs to the first vcg
   \param symbolic_trans_first : the transition with which we reached the first symbolic state
   \param symb_state_second : the symbolic state that belongs to the second vcg
   \param symbolic_trans_second : the transition with which we reached the second symbolic state
   \return a list of virtual constraints that are not bisimilar
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  check_for_virt_bisim(tchecker::zg::const_state_sptr_t symb_state_first, tchecker::zg::transition_sptr_t symbolic_trans_first,
                       tchecker::zg::const_state_sptr_t symb_state_second, tchecker::zg::transition_sptr_t symbolic_trans_second,
                       std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited);

  /*!
   \brief check-for-outgoing-transitions-impl function of Lieb et al.
   \param symb_state_first : the symbolic state that belongs to the first vcg
   \param vcg_first : the first vcg
   \param symb_state_second : the symbolic state that belongs to the second vcg
   \param vcg_second : the second vcg
   \param A_is_first : whether the outgoing transitions of A or B shall be checked
   \return a list of virtual constraints that cannot be simulated.
   \note the result is allocated at the heap and must be freed.
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  check_for_outgoing_transitions( tchecker::zg::const_state_sptr_t A_state,
                                  tchecker::zg::const_state_sptr_t B_state,
                                  std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited);

  const std::shared_ptr<tchecker::vcg::vcg_t> _A;
  const std::shared_ptr<tchecker::vcg::vcg_t> _B;

  long _visited_pair_of_states;

  long _delete_me;


};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
