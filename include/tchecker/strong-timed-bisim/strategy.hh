/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_STRATEGY_HH
#define TCHECKER_ALGORITHMS_STRATEGY_HH

#include "tchecker/strong-timed-bisim/non_bisim_cache.hh"
#include "tchecker/strong-timed-bisim/visited_map.hh"
#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace strong_timed_bisim {

/*!
 \class strategiy_t
 \brief This class replaces the certificates in case a set of symbolic states is given that should be checked.
 */
class strategy_t {
 public:
  /*!
   \brief Constructor
   \param _A : the first vcg
   \param _B : the second vcg
   \param symbolic_states_to_check : the symbolic states that should be checked
   */
  strategy_t(std::shared_ptr<tchecker::vcg::vcg_t> A, 
             std::shared_ptr<tchecker::vcg::vcg_t> B,
             std::vector<tchecker::zg::const_state_sptr_t> & symbolic_states_to_check);

  /*!
   \brief Adds the already checked symbolic states to the strategy
   \param non_bisim_cache : The cache that contains all checked symbolic states that are not bisimilar
   \param visited_map : The cache that contains all checked symbolic states that are bisimilar
   \post All checked symbolic states are part of this.
   */
  void insert_symb_states(std::shared_ptr<non_bisim_cache_t> non_bisim_cache, std::shared_ptr<visited_map_t> visited_map);

  /*!
   \brief Get a pair of symbolic states that is not contained by the strategy yet
   \param a reference to a pair, where the result is stored into
   \return true, if there exists such a pair. False, otherwise.
   */
  bool get_non_contained_states(std::pair<tchecker::zg::const_state_sptr_t, tchecker::zg::const_state_sptr_t> & result);

  /*!
   \brief Strategy output
   \param os : output stream
   \param name : graph name
   \post Strategy has been output to os
  */
  std::ostream & dot_output(std::ostream & os, std::string const & name);

 private:
  std::shared_ptr<tchecker::vcg::vcg_t> _A;
  std::shared_ptr<tchecker::vcg::vcg_t> _B;
  std::vector<tchecker::zg::const_state_sptr_t> _states_to_check;
  std::shared_ptr<non_bisim_cache_t> _non_bisim_cache;
  std::shared_ptr<visited_map_t> _visited_map;
};


} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif