/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_node.hh"
#include "tchecker/strong-timed-bisim/certificate/clock_names.hh"
#include "tchecker/operational-semantics/max_delay.hh"
#include "tchecker/operational-semantics/zone_valuation_converter.hh"
 
namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

node_t::node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
               std::shared_ptr<tchecker::clockval_t> valuation_1, std::shared_ptr<tchecker::clockval_t> valuation_2,
               std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2,
               std::size_t cut_off, bool urgent_clk_exists, std::size_t id, bool initial) :
               tchecker::strong_timed_bisim::certificate::node_t(location_pair, id, initial),
               _valuation(std::make_pair(valuation_1, valuation_2)),
               _invariant(std::make_pair(invariant_1, invariant_2)),
               _urgent_clk_exists(urgent_clk_exists),
               _cut_off(cut_off)
{
  assert(nullptr != _valuation.first);
  assert(nullptr != _valuation.second);
  assert(nullptr != _invariant.first);
  assert(nullptr != _invariant.second);
}

node_t::node_t(tchecker::ta::state_t & s_1, tchecker::ta::state_t & s_2,
               std::shared_ptr<tchecker::clockval_t> valuation_1, std::shared_ptr<tchecker::clockval_t> valuation_2,
               std::shared_ptr<tchecker::clock_constraint_container_t> invariant_1, std::shared_ptr<tchecker::clock_constraint_container_t> invariant_2,
               std::size_t cut_off, bool urgent_clk_exists, std::size_t id, bool initial) :
               tchecker::strong_timed_bisim::certificate::node_t(s_1, s_2, id, initial),
               _valuation(std::make_pair(valuation_1, valuation_2)),
               _invariant(std::make_pair(invariant_1, invariant_2)),
               _urgent_clk_exists(urgent_clk_exists), _final(false),
               _cut_off(cut_off)
{
  assert(nullptr != _valuation.first);
  assert(nullptr != _valuation.second);
  assert(nullptr != _invariant.first);
  assert(nullptr != _invariant.second);
}

node_t::node_t(const node_t & other) 
  : tchecker::strong_timed_bisim::certificate::node_t(*(other._location_pair), other._id, other._initial), 
    _invariant(other._invariant), _urgent_clk_exists(other._urgent_clk_exists), _final(other._final),
    _final_is_delay(other._final_is_delay), _final_delay(other._final_delay), _final_trans(other._final_trans),
    _final_first_has_transition(other._final_first_has_transition), _cut_off(other._cut_off)
{
  auto valuation_1 = clockval_factory(other._valuation.first->size());
  for(tchecker::clock_id_t i = 0; i < valuation_1->size(); i++) {
    (*valuation_1)[i] = (*other._valuation.first)[i];
  }

  auto valuation_2 = clockval_factory(other._valuation.second->size());
  for(tchecker::clock_id_t i = 0; i < valuation_2->size(); i++) {
    (*valuation_2)[i] = (*other._valuation.second)[i];
  }

  _valuation = std::make_pair(valuation_1, valuation_2);
}


bool node_t::operator==(node_t & other)
{
  bool result = tchecker::strong_timed_bisim::certificate::node_t::operator==(other);

  result &= this->_cut_off == other._cut_off;

  result &= other._valuation.first->size() == this->_valuation.second->size();
  result &= other._valuation.second->size() == this->_valuation.second->size();

  result &= tchecker::is_region_equivalent(*this->_valuation.first, *other._valuation.first, _cut_off);
  result &= tchecker::is_region_equivalent(*this->_valuation.second, *other._valuation.second, _cut_off);

  return result;
} 

void node_t::attributes(std::map<std::string, std::string> & m, const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                      const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const
{

  auto valuation_1 = clockval_factory(vcg1->get_no_of_original_clocks() + 1);
  for(tchecker::clock_id_t i = 0; i < valuation_1->size(); i++) {
    (*valuation_1)[i] = (*(_valuation.first))[i];
  }

  auto valuation_2 = clockval_factory(vcg2->get_no_of_original_clocks() + 1);
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
  auto cert_node = tchecker::strong_timed_bisim::certificate::node_t(symb_state_1, symb_state_2, 0);

  if(!(cert_node == *this)) {
    return false;
  }

  return symb_state_1->zone().belongs(*_valuation.first) && symb_state_2->zone().belongs(*_valuation.second);
}


bool node_t::is_leaf(std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
{
  assert(nullptr != vcg1);
  assert(nullptr != vcg2);
  assert(nullptr != _valuation.first);
  assert(nullptr != _valuation.second);
  assert(vcg1->get_no_of_virtual_clocks() == vcg2->get_no_of_virtual_clocks());
  assert(_valuation.first->size() == (2*vcg1->get_no_of_original_clocks() + vcg2->get_no_of_original_clocks() + 1 + (_urgent_clk_exists ? 1 : 0)));
  assert(_valuation.second->size() == (2*vcg2->get_no_of_original_clocks() + vcg1->get_no_of_original_clocks() + 1 + (_urgent_clk_exists ? 1 : 0)));
  
  if(!is_synchronized()) {
    return false;
  }
  
  auto zones = generate_zones(vcg1, vcg2);

  auto state_1 = vcg1->create_state(_location_pair->first.vloc(), _location_pair->first.intval(), *(zones->first));

  auto state_2 = vcg2->create_state(_location_pair->second.vloc(), _location_pair->second.intval(), *(zones->second));

  assert(state_2->zone().is_virtual_equivalent(state_1->zone(), vcg1->get_no_of_virtual_clocks()));

  auto state_1_fut = vcg1->clone_state(state_1);
  auto state_2_fut = vcg2->clone_state(state_2);

  if (tchecker::ta::delay_allowed(vcg1->system(), state_1_fut->vloc())) {
    vcg1->semantics()->delay(state_1_fut->zone_ptr()->dbm(), state_1_fut->zone_ptr()->dim(), *(_invariant.first));
  }

  if (tchecker::ta::delay_allowed(vcg2->system(), state_2_fut->vloc())) {
    vcg2->semantics()->delay(state_2_fut->zone_ptr()->dbm(), state_2_fut->zone_ptr()->dim(), *(_invariant.second));
  }

  if(!state_1_fut->zone().is_virtual_equivalent(state_2_fut->zone(), vcg1->get_no_of_virtual_clocks())) {
    auto first = tchecker::operational_semantics::max_delay(state_1_fut->zone(), _valuation.first, _cut_off, 0);
    auto second = tchecker::operational_semantics::max_delay(state_2_fut->zone(), _valuation.second, _cut_off, 0);
    _final = true;
    _final_first_has_transition = (second < first);

    double symbol = _final_first_has_transition ? 
                          static_cast<double>(first.value().numerator())/first.value().denominator() 
                        : static_cast<double>(second.value().numerator())/second.value().denominator();
    
    if((_final_first_has_transition && tchecker::operational_semantics::cmp_t::L == first.cmp()) ||
       (!_final_first_has_transition && tchecker::operational_semantics::cmp_t::L == second.cmp())) {
      symbol -= 0.5;
    }

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
  auto first_no_clks = vcg1->get_no_of_original_clocks() + vcg1->get_no_of_virtual_clocks() + 1;
  std::shared_ptr<tchecker::zg::zone_t> first = tchecker::zg::factory(first_no_clks);
  first->make_universal();
  tchecker::dbm::reduce_to_valuation(first->dbm(), *_valuation.first, first->dim());

  auto second_no_clks = vcg2->get_no_of_original_clocks() + vcg2->get_no_of_virtual_clocks() + 1;
  std::shared_ptr<tchecker::zg::zone_t> second = tchecker::zg::factory(second_no_clks);
  second->make_universal();
  tchecker::dbm::reduce_to_valuation(second->dbm(), *_valuation.second, second->dim());

  return std::make_shared<std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>>(std::pair(first, second));
}

std::pair<clock_rational_value_t, std::shared_ptr<node_t>>
node_t::max_delay(std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vcs, 
                  std::shared_ptr<tchecker::vcg::vcg_t> vcg1, std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
{
  std::pair<clock_rational_value_t, std::shared_ptr<node_t>> max = std::make_pair(clock_rational_value_t{0, 1}, nullptr);
  for(auto vc : *vcs) {
    std::pair<clock_rational_value_t, std::shared_ptr<node_t>> cur = this->max_delay(vc, vcg1, vcg2);
    if(cur.first > max.first) {
      max.first = cur.first;
      max.second = cur.second;
    }
  }

  return max;
}

std::pair<clock_rational_value_t, std::shared_ptr<node_t>>
node_t::max_delay(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc, 
                  std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                  std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
{
  assert(is_synchronized());

  std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>> zones =
      vc->generate_synchronized_zones(vcg1->get_no_of_original_clocks(), vcg2->get_no_of_original_clocks());

  auto new_valuation_1 = tchecker::clockval_factory(*_valuation.first);

  auto max_delay = tchecker::operational_semantics::max_delay(*zones.first, _valuation.first, _cut_off, 0);
  auto new_valuation_2 = tchecker::clockval_factory(*_valuation.second);

  clock_rational_value_t delay;

  if(tchecker::operational_semantics::cmp_t::L == max_delay.cmp()) {
    int64_t integral = max_delay.value().numerator() / max_delay.value().denominator();
    clock_rational_value_t fractional = max_delay.value() - integral;

    if(0 == fractional) {
      integral -= 1;
      fractional = 1;
    }

    clock_rational_value_t min = 0;
    std::vector<std::shared_ptr<tchecker::clockval_t>> vec;
    vec.emplace_back(new_valuation_1);
    vec.emplace_back(new_valuation_2);

    for(auto cur : vec) {
      for(std::size_t i = 0; i < cur->size(); i++) {
        int64_t int_val = ((*cur)[i].numerator() / (*cur)[i].denominator());
        clock_rational_value_t frac_val = (*cur)[i] - int_val;

        clock_rational_value_t new_lower_bound = 1 - frac_val;
        if(new_lower_bound < fractional) {
          min = std::max(min, new_lower_bound);
        }
      }
    }
    
    assert(fractional != min);

    clock_rational_value_t frac_delay = (fractional + min)/2;
    //std::cout << __FILE__ << ": " << __LINE__ << ": " << frac_delay << std::endl;

    delay = integral + frac_delay;

    add_delay(new_valuation_1, *_valuation.first,  delay);
    add_delay(new_valuation_2, *_valuation.second, delay);

  } else {
    delay = max_delay.value();
    add_delay(new_valuation_1, *_valuation.first,  delay);
    add_delay(new_valuation_2, *_valuation.second, delay);
  }

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