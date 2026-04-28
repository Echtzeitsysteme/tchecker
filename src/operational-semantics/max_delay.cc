/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/operational-semantics/max_delay.hh"

namespace tchecker {

namespace operational_semantics {

max_delay_t::max_delay_t(tchecker::clock_rational_value_t value, cmp_t cmp) : _value(value), _cmp(cmp)
{

}

max_delay_t
max_delay_helper(tchecker::zg::zone_t & zone, std::shared_ptr<tchecker::clockval_t> valuation, std::size_t max_delay_value, std::size_t min_delay_value)
{
  assert(min_delay_value >= 0); 
  assert(max_delay_value >= 0);
  assert(max_delay_value >= min_delay_value);

  auto clone = clockval_factory(valuation);
  add_delay(clone, *valuation, max_delay_value);

  if(zone.belongs(*clone)) { // if the maximum delay applied is still within zone, this is the maximum delay
    return max_delay_t(max_delay_value, cmp_t::LE);
  }

  add_delay(clone, *valuation, min_delay_value);

  if(!zone.belongs(*clone)) { // if the minimum delay applied is not within zone, this is not a valid range of delays. So return 0.
    return max_delay_t(0, cmp_t::LE);
  }

  if(min_delay_value + 1 == max_delay_value) {
    
    auto max_below = clock_rational_value_t(max_delay_value, 1) - clock_rational_value_t(1, 2);
    
    add_delay(clone, *valuation, max_below);

    if(zone.belongs(*clone)) {
      return max_delay_t(max_delay_value, cmp_t::L);
    } else {
      return max_delay_t(min_delay_value, cmp_t::LE);
    }
  }

  // if the min_delay_value is within the zone but max_delay_value is not, we have to check the value in the center
  std::size_t center = (max_delay_value + min_delay_value)/2;

  auto upper_result = max_delay_helper(zone, valuation, max_delay_value, center);

  auto lower_result = max_delay_helper(zone, valuation, center, min_delay_value);

  return std::max(upper_result, lower_result);
}

max_delay_t
max_delay(tchecker::zg::zone_t & zone, std::shared_ptr<tchecker::clockval_t> valuation, std::size_t max_delay_value, std::size_t min_delay_value)
{
  int64_t max_den = 1;
  for(auto i = 0; i < valuation->size(); i++) {
    max_den = std::max(max_den, (*valuation)[i].denominator());
  }

  // since we want to multiply all elements of the zone with that factor, we need to check whether 
  // the maximum denominator is within the bounds of int32_t
  if(max_den < std::numeric_limits<int32_t>::min() || max_den > std::numeric_limits<int32_t>::max()) {
    throw std::runtime_error("To small steps choosen");
  }

  auto clone_val = clockval_factory(valuation->size());
  for(auto i = 0; i < valuation->size(); i++) {
    (*clone_val)[i] = (*valuation)[i] * max_den;
  }

  auto clone_zone = tchecker::zg::factory(zone);
  tchecker::dbm::scale_up(clone_zone->dbm(), clone_zone->dim(), static_cast<int32_t>(max_den));

  auto calculated = max_delay_helper(*clone_zone, clone_val, max_den*max_delay_value, max_den*min_delay_value);
  return max_delay_t(calculated.value()/max_den, calculated.cmp());
  
}


} // end of namespace operational_semantics

} // end of namespace tchecker