/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

 #include "tchecker/strong-timed-bisim/contradiction.hh"

namespace tchecker{

namespace strong_timed_bisim {

contradiction_t::contradiction_t(tchecker::clock_id_t no_of_virt_clocks, tchecker::integer_t min_steps_to_cont) 
  : _contradictions(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virt_clocks+1)), 
    _min_steps_to_cont(min_steps_to_cont)
{

};


contradiction_t::contradiction_t(contradiction_t const & other) 
  : contradiction_t(other._contradictions, other._min_steps_to_cont)
{

};

contradiction_t::contradiction_t(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> contradictions, tchecker::integer_t min_steps_to_cont)
  : _contradictions(contradictions), _min_steps_to_cont(min_steps_to_cont)
{
  assert(std::all_of(contradictions->begin(), contradictions->end(), 
            [](const auto& vc) {
              return tchecker::dbm::is_consistent(vc->dbm(), vc->dim()) && tchecker::dbm::is_tight(vc->dbm(), vc->dim());
            }            
          ));
  _contradictions->compress();
  _contradictions = tchecker::virtual_constraint::combine(*_contradictions, _contradictions->dim() - 1);
  _contradictions->compress();
};

contradiction_t::contradiction_t(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> contradiction, tchecker::integer_t min_steps_to_cont)
  : _contradictions(std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(contradiction->dim())),
    _min_steps_to_cont(min_steps_to_cont)
{
  assert(tchecker::dbm::is_consistent(contradiction->dbm(), contradiction->dim()));
  assert(tchecker::dbm::is_tight(contradiction->dbm(), contradiction->dim()));
  _contradictions->append_zone(*contradiction);
  _contradictions->compress();
  _contradictions = tchecker::virtual_constraint::combine(*_contradictions, _contradictions->dim() - 1);
  _contradictions->compress();
}

bool contradiction_t::contradiction_free() 
{
  _contradictions->compress();
  return _contradictions->is_empty();
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> const
contradiction_t::get_contradictions() const
{
  return _contradictions;
}

void contradiction_t::add_contradiction(contradiction_t const & to_add)
{
  assert(_contradictions->dim() == to_add._contradictions->dim());

  _min_steps_to_cont = std::min(_min_steps_to_cont, to_add._min_steps_to_cont);

  _contradictions->append_container(to_add._contradictions);
  _contradictions->compress();
  _contradictions = tchecker::virtual_constraint::combine(*_contradictions, _contradictions->dim() - 1);
  _contradictions->compress();
}

/*< contradiction matrix >*/

} // end of namespace strong_timed_bisim

} // end of namespace tchecker