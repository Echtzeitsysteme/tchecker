/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"

// DELETE ME
#include <typeinfo>

namespace tchecker {

namespace strong_timed_bisim {

tchecker::strong_timed_bisim::stats_t Lieb_et_al::run(std::shared_ptr<tchecker::vcg::vcg_t> input_first, std::shared_ptr<tchecker::vcg::vcg_t> input_second) {

  std::cout << "run algorithm" << std::endl;

  stats_t stats;
  stats.set_start_time();

  // sst is a tuple (tchecker::state_status_t, state_t, transition_t)
  // state_status_t : see basictypes.hh
  // tchecker::zg::state_sptr_t (= tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_state_t>;)

  std::vector<tchecker::zg::zg_t::sst_t> sst_first;
  std::vector<tchecker::zg::zg_t::sst_t> sst_second;

  input_first->initial(sst_first);
  input_second->initial(sst_second);

  if(STATE_OK != std::get<0>(sst_first[0]) || STATE_OK != std::get<0>(sst_second[0])) 
    throw std::runtime_error("problems with initial state");

  std::size_t dim = std::get<1>(sst_first[0])->zone().dim();
  auto dbm = std::get<1>(sst_first[0])->zone().dbm();

  for(std::size_t i = 0; i < dim; ++i) {
      for(std::size_t j = 0; j < dim; ++j) {
        std::size_t index = i*dim + j;
        std::cout << "(" << dbm[index].cmp << ", " << dbm[index].value << ") ";
      }
      std::cout << std::endl;
  }

  

  stats.set_end_time();

  return stats;
}

} // enf of namespace strong_timed_bisim

} // end ofnamespace tchecker

