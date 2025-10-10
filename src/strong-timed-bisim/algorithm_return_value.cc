/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

 #include "tchecker/strong-timed-bisim/algorithm_return_value.hh"

namespace tchecker{

namespace strong_timed_bisim {

algorithm_return_value_t::algorithm_return_value_t(tchecker::clock_id_t no_of_virt_clocks, 
                                                   tchecker::zg::const_state_sptr_t A_state, 
                                                   tchecker::zg::const_state_sptr_t B_state) : 
                                                    _contradictions(
                                                      std::make_shared<
                                                        tchecker::zone_container_t<
                                                          tchecker::virtual_constraint::virtual_constraint_t
                                                        >
                                                      >(no_of_virt_clocks+1)),
                                                    _A_state_vc(tchecker::virtual_constraint::factory(A_state->zone(), no_of_virt_clocks)), 
                                                    _B_state_vc(tchecker::virtual_constraint::factory(B_state->zone(), no_of_virt_clocks))
{

};

algorithm_return_value_t::algorithm_return_value_t(tchecker::clock_id_t no_of_virt_clocks, 
                                                   tchecker::zg::zone_t const & A_zone, 
                                                   tchecker::zg::zone_t const & B_zone) : 
                                                    _contradictions(
                                                      std::make_shared<
                                                        tchecker::zone_container_t<
                                                          tchecker::virtual_constraint::virtual_constraint_t
                                                        >
                                                      >(no_of_virt_clocks+1)),
                                                    _A_state_vc(tchecker::virtual_constraint::factory(A_zone, no_of_virt_clocks)), 
                                                    _B_state_vc(tchecker::virtual_constraint::factory(B_zone, no_of_virt_clocks))
{

};

algorithm_return_value_t::algorithm_return_value_t(algorithm_return_value_t& other) : 
                                                  _contradictions(other._contradictions), 
                                                  _A_state_vc(other._A_state_vc), 
                                                  _B_state_vc(other._B_state_vc)
{

};

algorithm_return_value_t::algorithm_return_value_t(
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions,
  tchecker::zg::const_state_sptr_t A_state, 
  tchecker::zg::const_state_sptr_t B_state) : 
  _contradictions(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*contradictions)),
  _A_state_vc(tchecker::virtual_constraint::factory(A_state->zone(), contradictions->dim() - 1)), 
  _B_state_vc(tchecker::virtual_constraint::factory(B_state->zone(), contradictions->dim() - 1))
{

};

algorithm_return_value_t::algorithm_return_value_t(
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions,
  tchecker::zg::zone_t const & A_zone, 
  tchecker::zg::zone_t const & B_zone) : 
  _contradictions(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*contradictions)),
  _A_state_vc(tchecker::virtual_constraint::factory(A_zone, contradictions->dim() - 1)), 
  _B_state_vc(tchecker::virtual_constraint::factory(B_zone, contradictions->dim() - 1))
{
 assert(std::all_of(
          contradictions->begin(), contradictions->end(),
          [this](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> cur_con) 
          { return *cur_con <= *(this->_A_state_vc) || *cur_con <= *(this->_B_state_vc); }));
};

bool algorithm_return_value_t::contradiction_free() 
{
  _contradictions->compress();
  return _contradictions->is_empty();
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> const
algorithm_return_value_t::get_contradictions()
{
  return _contradictions;
}

void algorithm_return_value_t::add_to_contradictions(tchecker::virtual_constraint::virtual_constraint_t& to_add)
{
  assert(_contradictions->dim() == to_add.get_no_of_virtual_clocks() + 1);

  assert(to_add <= *_A_state_vc || to_add <= *_B_state_vc);

  _contradictions->append_zone(to_add);
  _contradictions->compress();
  _contradictions = tchecker::virtual_constraint::combine(*_contradictions, to_add.get_no_of_virtual_clocks());
  _contradictions->compress();
}

void algorithm_return_value_t::add_to_contradictions(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_add)
{
  this->add_to_contradictions(*to_add);
}

void algorithm_return_value_t::add_to_contradictions(tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>& to_add){
  assert(_contradictions->dim() == to_add.dim());
  assert(std::all_of(
          to_add.begin(), to_add.end(),
          [this](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> cur_con) 
          { return *cur_con <= *(this->_A_state_vc) || *cur_con <= *(this->_B_state_vc); }));

  _contradictions->append_container(to_add);
  _contradictions->compress();
  _contradictions = tchecker::virtual_constraint::combine(*_contradictions, _contradictions->dim() - 1); // compress needs the no of virt clks
  _contradictions->compress();
}

void algorithm_return_value_t::add_to_contradictions(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_add){
  this->add_to_contradictions(*to_add);
}

}

}