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
Lieb_et_al::check_for_outgoing_transitions( std::shared_ptr<tchecker::vcg::state_t const> symb_state_A,
                                            std::shared_ptr<tchecker::vcg::state_t const> symb_state_B,
                                            bool A_notB,
                                            std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
                                                  (*func)(std::shared_ptr<tchecker::vcg::state_t>, std::shared_ptr<tchecker::vcg::state_t>))
{
#if 0
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> result = std::make_shared<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(this->_A->get_no_of_virtual_clocks());

  std::shared_ptr<tchecker::vcg::vcg_t> vcg_to_check;
  std::shared_ptr<tchecker::vcg::vcg_t> vcg_checker;

  tchecker::intrusive_shared_ptr_t<tchecker::vcg::state_t const> state_to_check;
  std::shared_ptr<tchecker::vcg::state_t const> state_checker;

  if(A_notB) {
    vcg_to_check = _A;
    vcg_checker = _B;
    state_to_check = symb_state_A;
    state_checker = symb_state_B;
  } else {
    vcg_to_check = _B;
    vcg_checker = _A;
    state_to_check = symb_state_B;
    state_checker = symb_state_A;
  }

  std::vector<tchecker::zg::zg_t::sst_t> transitions_to_check;

  tchecker::zg::outgoing_edges_range_t edges_to_check = tchecker::zg::outgoing_edges(*(vcg_to_check->get_system()), state_to_check->vloc_ptr());

  for(tchecker::zg::outgoing_edges_value_t && out_edges : edges_to_check) {
      vcg_to_check->next(state_to_check, transitions_to_check, tchecker::STATE_OK);
  }
#endif
}

} // end of namespace strong_timed_bisim

} // end ofnamespace tchecker

