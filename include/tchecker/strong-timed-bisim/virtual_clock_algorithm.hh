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
#include "tchecker/zg/zone_container.hh"

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
   \brief running the algorithm of Lieb et al.
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
   \brief checks whether we need to do an epsilon transition
   \param A_state : first state
   \param B_state : second state
   \Return true if the states are either not synced or the result of a delay is different than the original symbolic states
  */
  bool do_an_epsilon_transition(tchecker::zg::state_sptr_t A_state, tchecker::zg::transition_sptr_t A_trans,
                                tchecker::zg::state_sptr_t B_state, tchecker::zg::transition_sptr_t B_trans);

  /*!
   \brief check-for-virt-bisim function of Lieb et al.
   \param symb_state_first : the symbolic state that belongs to the first vcg
   \param A_trans : the transition with which we reached the first symbolic state
   \param symb_state_second : the symbolic state that belongs to the second vcg
   \param B_trans : the transition with which we reached the second symbolic state
   \param last_was_epsilon : whether the last transition was an epsilon transition
   \param visited : a set of assumptions
   \return a list of virtual constraints that are not bisimilar
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  check_for_virt_bisim(tchecker::zg::const_state_sptr_t A_state, tchecker::zg::transition_sptr_t A_trans,
                       tchecker::zg::const_state_sptr_t B_state, tchecker::zg::transition_sptr_t B_trans,
                       std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited);

  /*!
   \brief : removes found contradictions from a zone
   \param zone : the zone to constraint
   \param contradictions : the virtual constraints that shall be removed from zone
   \return virtual_constraint of zone && (! && of all elements in contradictions)
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  extract_vc_without_contradictions(tchecker::zg::zone_t const & zone, std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions);

  /*!
   \brief : takes two transitions and splits of the target zones. Returns a contradiction if one is found.
   \param : zones-A : the splits of the target zones of trans_A
   \param : trans_A : the first transition
   \param : zones_B : the splits of the target zones of trans_B
   \param : trans_B : the second transition
   \param : visted : the assumptions.
   \return a contradiction if there is one. Otherwise an empty list of virtual constraints.
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  check_target_pair(tchecker::zg::state_sptr_t target_state_A, tchecker::zg::transition_sptr_t trans_A,
                    tchecker::zg::state_sptr_t target_state_B, tchecker::zg::transition_sptr_t trans_B,
                    std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> already_found_contradictions,
                    std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited);

  /*!
   \brief check-for-outgoing-transitions-impl function of Lieb et al.
   \param zone_A : the zone of the symbolic state that belongs to the first vcg
   \param zone_B : the zone of the symbolic state that belongs to the second vcg
   \param trans_A : the list of transitions that belongs to the first vcg
   \param trans_B : the list of transitions that belongs to the second vcg
   \param visited : the assumptions
   \return a list of virtual constraints that cannot be simulated.
   \note the result is allocated at the heap and must be freed.
   */
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  check_for_outgoing_transitions( tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                  std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_A, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_B,
                                  std::unordered_set<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>, custom_hash, custom_equal> & visited);

  const std::shared_ptr<tchecker::vcg::vcg_t> _A;
  const std::shared_ptr<tchecker::vcg::vcg_t> _B;

  long _visited_pair_of_states;

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
