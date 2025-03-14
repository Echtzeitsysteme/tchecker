/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

 #include "tchecker/strong-timed-bisim/algorithm_return_value.hh"

namespace tchecker{

namespace strong_timed_bisim {

algorithm_return_value_t::algorithm_return_value_t(tchecker::clock_id_t no_of_virt_clocks) : 
    _contradictions(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clocks+1)), 
    _check(no_of_virt_clocks) { };

algorithm_return_value_t::algorithm_return_value_t(algorithm_return_value_t& other) : _contradictions(other._contradictions), _check(other._check) { };

algorithm_return_value_t::algorithm_return_value_t(
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions,
  std::shared_ptr<tchecker::strong_timed_bisim::visited_map_t> check) : 
  _contradictions(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*contradictions)), 
  _check(*check) { };

bool algorithm_return_value_t::contradiction_free() 
{
  _contradictions->compress();
  return _contradictions->is_empty();
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
algorithm_return_value_t::get_contradictions() 
{
  return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*_contradictions);
}

std::shared_ptr<tchecker::strong_timed_bisim::visited_map_t>
algorithm_return_value_t::get_check() 
{
  return std::make_shared<tchecker::strong_timed_bisim::visited_map_t>(_check);
}

void algorithm_return_value_t::add_to_contradictions(tchecker::virtual_constraint::virtual_constraint_t& to_add)
{
  assert(_contradictions->dim() == to_add.get_no_of_virtual_clocks() + 1);
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
  _contradictions->append_container(to_add);
  _contradictions->compress();
  _contradictions = tchecker::virtual_constraint::combine(*_contradictions, _contradictions->dim() - 1); // compress needs the no of virt clks
  _contradictions->compress();
}

void algorithm_return_value_t::add_to_contradictions(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_add){
  this->add_to_contradictions(*to_add);
}

void algorithm_return_value_t::add_to_check(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second)
{
  _check.emplace(first, second);
}

}

}