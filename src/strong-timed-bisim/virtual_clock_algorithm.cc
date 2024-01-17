/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

Lieb_et_al::Lieb_et_al(std::shared_ptr<tchecker::vcg::vcg_t> input_first, std::shared_ptr<tchecker::vcg::vcg_t> input_second) : _A(input_first), _B(input_first) {}

tchecker::strong_timed_bisim::stats_t Lieb_et_al::run() {

  std::cout << "run algorithm" << std::endl;

  stats_t stats;
#if 0
  stats.set_start_time();

  // sst is a tuple (tchecker::state_status_t, state_t, transition_t)
  // state_status_t : see basictypes.hh

  std::vector<tchecker::zg::zg_t::sst_t> sst_first;
  std::vector<tchecker::zg::zg_t::sst_t> sst_second;

  this->_A->initial(sst_first);
  this->_B->initial(sst_second);

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

  tchecker::virtual_constraint::virtual_constraint_t *vc = tchecker::virtual_constraint::factory(&(std::get<1>(sst_first[0])->zone()), this->_A->get_no_of_virtual_clocks());

  const clock_constraint_container_t & virt_cons = vc->get_vc(this->_A->clocks_count() - this->_A->get_no_of_virtual_clocks());

  for(auto iter = virt_cons.begin(); iter < virt_cons.end(); iter++) {
    std::cout << *iter << std::endl;
  }
/*
  for(std::size_t i = 0; i <= input_first->get_no_of_virtual_clocks(); ++i) {
      for(std::size_t j = 0; j < input_first->get_no_of_virtual_clocks(); ++j) {
        std::size_t index = i*input_first->get_no_of_virtual_clocks() + i + j;
        std::cout << "(" << (vc->get_dbm())[index].cmp << ", " << (vc->get_dbm())[index].value << ") ";
      }
      std::cout << std::endl;
  }
*/
  delete vc;

  stats.set_end_time();
#endif
  return stats;

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
Lieb_et_al::check_for_outgoing_transitions( tchecker::zg::const_state_sptr_t symb_state_first,
                                            std::shared_ptr<tchecker::vcg::vcg_t> vcg_first,
                                            tchecker::zg::const_state_sptr_t symb_state_second,
                                            std::shared_ptr<tchecker::vcg::vcg_t> vcg_second,
                                            std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
                                              (*func)(tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t))
{

  std::vector<tchecker::vcg::vcg_t::sst_t> v_first;
  vcg_first->next(symb_state_first, v_first);

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result
     = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(vcg_first->get_no_of_virtual_clocks());

  for (auto && [status_first, s_first, t_first] : v_first) {

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> lo_result;

    std::vector<tchecker::vcg::vcg_t::sst_t> v_second;
    vcg_second->next(symb_state_second, v_second);

    for (auto && [status_second, s_second, t_second] : v_first) {

      if(t_first->vedge().event_equal(vcg_first->system(), t_second->vedge(), vcg_second->system())) {
        std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> inter = func(s_first, s_second);
        for(auto iter = inter->begin(); iter < inter->end(); iter++) {
          // TODO
        }
      }

    }
  }
}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

