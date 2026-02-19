/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/operational-semantics/max_delay.hh"

namespace tchecker {

namespace operational_semantics {

clock_rational_value_t
max_delay(tchecker::zg::zone_t & zone, std::shared_ptr<tchecker::clockval_t> valuation, std::size_t max_delay_value, std::size_t min_delay_value)
{
  assert(min_delay_value >= 0); 
  assert(max_delay_value >= 0);
  assert(max_delay_value >= min_delay_value);

  tchecker::clockval_t *raw = tchecker::clockval_clone(*valuation);
  auto clone = std::shared_ptr<tchecker::clockval_t>(raw, &clockval_destruct_and_deallocate);
  add_delay(clone, *valuation, max_delay_value);

  if(zone.belongs(*clone)) { // if the maximum delay applied is still within zone, this is the maximum delay
    return max_delay_value;
  }

  add_delay(clone, *valuation, min_delay_value);

  if(!zone.belongs(*clone)) { // if the minimum delay applied is not within zone, this is not a valid range of delays. So return 0.
    return 0;
  }

  if(min_delay_value + 1 == max_delay_value) { // if the min_delay_value is within the zone but max_delay_value is not, we have to check the value in the center
    
    auto zero_five = clock_rational_value_t(min_delay_value, 1) + clock_rational_value_t(1, 2); // clock_rational_value_t(1, 2) = 1/2 = 0.5
    
    add_delay(clone, *valuation, zero_five);

    if(zone.belongs(*clone)) {
      return zero_five;
    } else {
      return min_delay_value;
    }
  }

  std::size_t center = (max_delay_value + min_delay_value)/2;

  auto upper_result = max_delay(zone, valuation, max_delay_value, center);

  auto lower_result = max_delay(zone, valuation, center, min_delay_value);

  return std::max(upper_result, lower_result);
}

} // end of namespace operational_semantics

} // end of namespace tchecker