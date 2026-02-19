/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_node.hh"

#include "tchecker/strong-timed-bisim/certificate/clock_names.hh"

#include "tchecker/operational-semantics/max_delay.hh"
 
namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

node_t::node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
               std::shared_ptr<tchecker::clockval_t> valuation_1, std::shared_ptr<tchecker::clockval_t> valuation_2,
               std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2,
               std::size_t id, bool urgent_clk_exists, bool initial) :
               tchecker::strong_timed_bisim::certificate::node_t(location_pair, id, initial),
               _valuation(std::make_pair(valuation_1, valuation_2)),
               _invariant(std::make_pair(invariant_1, invariant_2)),
               _urgent_clk_exists(urgent_clk_exists)
{
}

node_t::node_t(tchecker::zg::state_sptr_t s_1, tchecker::zg::state_sptr_t s_2,
               tchecker::clock_id_t no_of_orig_clks_1, tchecker::clock_id_t no_of_orig_clks_2,
               std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2,
               std::size_t id, bool urgent_clk_exists, bool initial) :
               tchecker::strong_timed_bisim::certificate::node_t(s_1, s_2, id, initial),
               _invariant(std::make_pair(invariant_1, invariant_2)),
               _urgent_clk_exists(urgent_clk_exists), _final(false)
{
  auto raw_1 = clockval_allocate_and_construct(2*no_of_orig_clks_1 + no_of_orig_clks_2 + 1 + (_urgent_clk_exists ? 1 : 0)); // +1 due to the ref clock
  auto valuation_1 = std::shared_ptr<tchecker::clockval_t>(raw_1, &clockval_destruct_and_deallocate);
  for(tchecker::clock_id_t i = 0; i < valuation_1->size(); i++) {
    (*valuation_1)[i] = 0;
  }

  
  auto raw_2 = clockval_allocate_and_construct(2*no_of_orig_clks_2 + no_of_orig_clks_1 + 1 + (_urgent_clk_exists ? 1 : 0)); // +1 due to the ref clock
  auto valuation_2 = std::shared_ptr<tchecker::clockval_t>(raw_2, &clockval_destruct_and_deallocate);
  for(tchecker::clock_id_t i = 0; i < valuation_2->size(); i++) {
    (*valuation_2)[i] = 0;
  }

  _valuation = std::make_pair(valuation_1, valuation_2);
}

node_t::node_t(const node_t & other) 
  : tchecker::strong_timed_bisim::certificate::node_t(*(other._location_pair), 0, other._initial), 
    _invariant(other._invariant), _urgent_clk_exists(other._urgent_clk_exists), _final(other._final),
    _final_is_delay(other._final_is_delay), _final_delay(other._final_delay), _final_trans(other._final_trans),
    _final_first_has_transition(other._final_first_has_transition)
{
  auto raw_1 = clockval_allocate_and_construct(other._valuation.first->size());
  auto valuation_1 = std::shared_ptr<tchecker::clockval_t>(raw_1, &clockval_destruct_and_deallocate);
  for(tchecker::clock_id_t i = 0; i < valuation_1->size(); i++) {
    (*valuation_1)[i] = (*other._valuation.first)[i];
  }

  
  auto raw_2 = clockval_allocate_and_construct(other._valuation.second->size()); // +1 due to the ref clock
  auto valuation_2 = std::shared_ptr<tchecker::clockval_t>(raw_2, &clockval_destruct_and_deallocate);
  for(tchecker::clock_id_t i = 0; i < valuation_2->size(); i++) {
    (*valuation_2)[i] = (*other._valuation.second)[i];
  }

  _valuation = std::make_pair(valuation_1, valuation_2);
}


bool node_t::operator==(node_t & other)
{
  bool result = tchecker::strong_timed_bisim::certificate::node_t::operator==(other);

  result &= (0 == tchecker::lexical_cmp(*(this->valuation().first), *(other.valuation().first)));
  result &= (0 == tchecker::lexical_cmp(*(this->valuation().second), *(other.valuation().second)));

  return result;
} 

void node_t::attributes(std::map<std::string, std::string> & m, const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                      const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
{

  auto raw_1 = clockval_allocate_and_construct(vcg1->get_no_of_original_clocks() + 1);
  auto valuation_1 = std::shared_ptr<tchecker::clockval_t>(raw_1, &clockval_destruct_and_deallocate);
  for(tchecker::clock_id_t i = 0; i < valuation_1->size(); i++) {
    (*valuation_1)[i] = (*(_valuation.first))[i];
  }

  auto raw_2 = clockval_allocate_and_construct(vcg2->get_no_of_original_clocks() + 1);
  auto valuation_2 = std::shared_ptr<tchecker::clockval_t>(raw_2, &clockval_destruct_and_deallocate);
  for(tchecker::clock_id_t i = 0; i < valuation_2->size(); i++) {
    (*valuation_2)[i] = (*(_valuation.second))[i];
  }


  m["clockval_1"] = to_string(*valuation_1, tchecker::strong_timed_bisim::certificate::clock_names(vcg1, "_1"));
  m["clockval_2"] = to_string(*valuation_2, tchecker::strong_timed_bisim::certificate::clock_names(vcg2, "_2"));

  if(_final) {
    m["final"] = _final_first_has_transition ? "first" : "second";
    if(_final_is_delay) {
      m["final_delay"] = _final_delay;
    } else {
      m["final_edge"] = tchecker::to_string(_final_trans->vedge(), 
                                            (_final_first_has_transition) ? 
                                              (vcg1->system().as_system_system()) : (vcg2->system().as_system_system()));
    }
  }

  tchecker::strong_timed_bisim::certificate::node_t::attributes(m, vcg1, vcg2);
}

bool node_t::is_synchronized() const
{
  // we know that: _valuation_1->size() = 2*orig_clocks_1 + orig_clocks_2 + 1
  // and:          _valuation_2->size() = orig_clocks_1 + 2*orig_clocks_2 + 1
  // However, there might exists an urgent clk. In this case, there is one additional virtual clock
  // Therefore: 
  unsigned short orig_clocks_1 = std::round((2*_valuation.first->size() - _valuation.second->size() - 1 - (_urgent_clk_exists ? 1 : 0))/3);
  unsigned short orig_clocks_2 = std::round((2*_valuation.second->size() - _valuation.first->size() - 1 - (_urgent_clk_exists ? 1 : 0))/3);

  for(unsigned short i = 1; i <=orig_clocks_1; ++i) {
    if((*_valuation.first)[i] != (*_valuation.first)[orig_clocks_1 + i]) {
      return false;
    }
  }

  for(unsigned short i = 1; i <= orig_clocks_2; ++i) {
    if((*_valuation.second)[i] != (*_valuation.second)[orig_clocks_2 + orig_clocks_1 + i]) {
      return false;
    }
  }

  for(unsigned short i = 1; i <= orig_clocks_1 + orig_clocks_2; ++i) {
    if((*_valuation.first)[orig_clocks_1 + i] != (*_valuation.second)[orig_clocks_2 + i]) {
      return false;
    }
  }

  if(_urgent_clk_exists) {
    return (*_valuation.first)[2*orig_clocks_1 + orig_clocks_2] == (*_valuation.second)[2*orig_clocks_2 + orig_clocks_1];
  }

  return true;
}

void node_t::synchronize()
{
  unsigned short orig_clocks_1 = std::round((2*_valuation.first->size() - _valuation.second->size() - 1 - (_urgent_clk_exists ? 1 : 0))/3);
  unsigned short orig_clocks_2 = std::round((2*_valuation.second->size() - _valuation.first->size() - 1 - (_urgent_clk_exists ? 1 : 0))/3);

  for (unsigned short i = 1; i <= orig_clocks_1; ++i) {
    if ((*_valuation.first)[i] == 0 && (*_valuation.first)[orig_clocks_1 + i] != 0) {
      (*_valuation.first)[orig_clocks_1 + i] = 0;
      (*_valuation.second)[orig_clocks_2 + i] = 0;
    }
  }

  for (unsigned short i = 1; i <= orig_clocks_2; ++i) {
    if ((*_valuation.second)[i] == 0 && (*_valuation.second)[orig_clocks_2 + orig_clocks_1 + i] != 0) {
      (*_valuation.first)[orig_clocks_1 + orig_clocks_1 + i] = 0;
      (*_valuation.second)[orig_clocks_2 + orig_clocks_1 + i] = 0;
    }
  }

  if (_urgent_clk_exists && (*_valuation.first)[2 * orig_clocks_1 + orig_clocks_2] != (*_valuation.second)[2 * orig_clocks_2 + orig_clocks_1]) {
    (*_valuation.first)[2 * orig_clocks_1 + orig_clocks_2] = 0;
    (*_valuation.second)[2 * orig_clocks_2 + orig_clocks_1] = 0;
  }

  assert(is_synchronized());
}

bool node_t::is_element_of(tchecker::zg::state_sptr_t symb_state_1, tchecker::zg::state_sptr_t symb_state_2) const
{
  auto cert_node = tchecker::strong_timed_bisim::certificate::node_t(symb_state_1, symb_state_2);

  if(!(cert_node == *this)) {
    return false;
  }

  return symb_state_1->zone().belongs(*_valuation.first) && symb_state_2->zone().belongs(*_valuation.second);
}


bool node_t::is_leaf(tchecker::zg::state_sptr_t & init_1, tchecker::zg::state_sptr_t & init_2, 
                     std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2, 
                     std::size_t max_possible_delay)
{
  assert(vcg1->get_no_of_virtual_clocks() == vcg2->get_no_of_virtual_clocks());
  assert(_valuation.first->size() == (2*vcg1->get_no_of_original_clocks() + vcg2->get_no_of_original_clocks() + 1 + (_urgent_clk_exists ? 1 : 0)));
  assert(_valuation.second->size() == (2*vcg2->get_no_of_original_clocks() + vcg1->get_no_of_original_clocks() + 1 + (_urgent_clk_exists ? 1 : 0)));
  
  if(!is_synchronized()) {
    return false;
  }
  
  auto zones = generate_zones(vcg1, vcg2);

  auto state_1 = vcg1->clone_state(init_1);
  tchecker::dbm::copy(state_1->zone().dbm(), zones->first->dbm(), zones->first->dim());
  auto state_1_fut = vcg1->clone_state(state_1);

  auto state_2 = vcg2->clone_state(init_2);
  tchecker::dbm::copy(state_2->zone().dbm(), zones->second->dbm(), zones->second->dim());
  auto state_2_fut = vcg2->clone_state(state_2);


  assert(state_2->zone().is_virtual_equivalent(state_1->zone(), vcg1->get_no_of_virtual_clocks()));

  if (tchecker::ta::delay_allowed(vcg1->system(), state_1_fut->vloc())) {
    vcg1->semantics()->delay(state_1_fut->zone_ptr()->dbm(), state_1_fut->zone_ptr()->dim(), *(_invariant.first));
  }

  if (tchecker::ta::delay_allowed(vcg2->system(), state_2_fut->vloc())) {
    vcg2->semantics()->delay(state_2_fut->zone_ptr()->dbm(), state_2_fut->zone_ptr()->dim(), *(_invariant.second));
  }

  if(!state_1_fut->zone().is_virtual_equivalent(state_2_fut->zone(), vcg1->get_no_of_virtual_clocks())) {
    auto first = tchecker::operational_semantics::max_delay(state_1_fut->zone(), _valuation.first, max_possible_delay, 0);
    auto second = tchecker::operational_semantics::max_delay(state_2_fut->zone(), _valuation.second, max_possible_delay, 0);
    _final = true;
    _final_first_has_transition = (first > second);
    double symbol = _final_first_has_transition ? static_cast<double>(first.numerator()) / first.denominator() : static_cast<double>(second.numerator()) / second.denominator();
    symbol *= 10;
    int symbol_cut = std::round(symbol);
    
    std::ostringstream oss;
    oss << (symbol_cut/10);

    _final_is_delay = true;
    _final_delay = oss.str();
    return true;
  }

  auto avail_events_1 = std::make_shared<std::set<std::set<std::string>>>();
  auto avail_events_2 = std::make_shared<std::set<std::set<std::string>>>();

  vcg1->avail_events(avail_events_1, state_1);
  vcg2->avail_events(avail_events_2, state_2);

  if(*avail_events_1 != *avail_events_2) {
    _final = true;
    _final_is_delay = false;
    std::set<std::set<std::string>> diff;

    std::set_difference(
        avail_events_1->begin(), avail_events_1->end(),
        avail_events_2->begin(), avail_events_2->end(),
        std::inserter(diff, diff.begin())
    );

    _final_first_has_transition = !diff.empty();

    if(diff.empty()) {
      std::set_difference(
        avail_events_2->begin(), avail_events_2->end(),
        avail_events_1->begin(), avail_events_1->end(),
        std::inserter(diff, diff.begin())
      );
      assert(!diff.empty()); // if the sets are not equal, either the first or second diff must be non-empty
      _final_trans = vcg2->edge_of_event(state_2, *diff.begin());
    } else {
      _final_trans = vcg1->edge_of_event(state_1, *diff.begin());
    }
    return true;
  }

  return false;
}


std::shared_ptr<std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>>
node_t::generate_zones(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
{
  std::shared_ptr<tchecker::zg::zone_t> first = tchecker::zg::factory(vcg1->get_no_of_original_clocks() + vcg1->get_no_of_virtual_clocks() + 1);
  first->make_universal();
  tchecker::dbm::reduce_to_valuation(first->dbm(), *_valuation.first, first->dim());

  std::shared_ptr<tchecker::zg::zone_t> second = tchecker::zg::factory(vcg2->get_no_of_original_clocks() + vcg2->get_no_of_virtual_clocks() + 1);
  second->make_universal();
  tchecker::dbm::reduce_to_valuation(second->dbm(), *_valuation.second, second->dim());

  return std::make_shared<std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>>(std::pair(first, second));
}

std::pair<clock_rational_value_t, std::shared_ptr<node_t>>
node_t::max_delay(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vcs, 
                  std::size_t max_delay_value, 
                  std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
{
  std::pair<clock_rational_value_t, std::shared_ptr<node_t>> max = std::make_pair(clock_rational_value_t{0, 1}, nullptr);
  for(auto vc : *vcs) {
    std::pair<clock_rational_value_t, std::shared_ptr<node_t>> cur = this->max_delay(vc, max_delay_value, vcg1, vcg2);
    if(cur.first > max.first) {
      max.first = cur.first;
      max.second = cur.second;
    }
  }

  return max;
}

std::pair<clock_rational_value_t, std::shared_ptr<node_t>>
node_t::max_delay(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc, 
                  std::size_t max_delay_value, 
                  std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                  std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
{
  assert(is_synchronized());

  std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>> zones =
      vc->generate_synchronized_zones(vcg1->get_no_of_original_clocks(), vcg2->get_no_of_original_clocks());
  
  clock_rational_value_t delay = tchecker::operational_semantics::max_delay(*zones.first, _valuation.first, max_delay_value, 0);

  auto clone_1 = tchecker::clockval_clone(*_valuation.first);
  auto new_valuation_1 = std::shared_ptr<tchecker::clockval_t>(clone_1, &clockval_destruct_and_deallocate);
  add_delay(new_valuation_1, *_valuation.first, delay);


  auto clone_2 = tchecker::clockval_clone(*_valuation.second);
  auto new_valuation_2 = std::shared_ptr<tchecker::clockval_t>(clone_2, &clockval_destruct_and_deallocate);
  add_delay(new_valuation_2, *_valuation.second, delay);

  std::shared_ptr<node_t> result = std::make_shared<node_t>(*this);
  result->set_initial(false);
  result->set_valuation(std::make_pair(new_valuation_1, new_valuation_2));

  return std::make_pair(delay, result);
}

void node_t::set_valuation(std::pair<std::shared_ptr<tchecker::clockval_t>, std::shared_ptr<tchecker::clockval_t>> new_val)
{
  assert(new_val.first->size() == _valuation.first->size());
  assert(new_val.second->size() == _valuation.second->size());

  for(unsigned short i = 0; i < new_val.first->size(); ++i) {
    (*_valuation.first)[i] = (*new_val.first)[i];
  }

  for(unsigned short i = 0; i < new_val.second->size(); ++i) {
    (*_valuation.second)[i] = (*new_val.second)[i];
  }
}

  void node_t::reset_value(bool first_not_second, tchecker::clock_id_t i)
  {
    assert(i < (first_not_second ? _valuation.first->size() : _valuation.second->size()));
    if(first_not_second) {
      (*_valuation.first)[i] = 0;
    } else {
      (*_valuation.second)[i] = 0;
    }
  }

}// end of namespace contra

} // end of namespace strong_timed_bisim

} // end of namespace tchecker