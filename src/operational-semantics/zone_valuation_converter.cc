/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/operational-semantics/zone_valuation_converter.hh"

namespace tchecker {

namespace operational_semantics {

void compress_zone(std::shared_ptr<tchecker::zg::zone_t> zone)
{
  for(std::size_t i = 0; i < zone->dim(); i++) {
    auto upper_bound = tchecker::dbm::access(zone->dbm(), zone->dim(), i, 0);
    auto lower_bound = tchecker::dbm::access(zone->dbm(), zone->dim(), 0, i);

    // first check whether clock i can only have one value
    if(tchecker::LE == upper_bound->cmp && tchecker::LE == lower_bound->cmp && (-1)*lower_bound->value == upper_bound->value) {
      continue;
    }
    // Now check whether clock i can only have values between value and value+1
    if(tchecker::LT == upper_bound->cmp && tchecker::LT == lower_bound->cmp && (-1)*lower_bound->value + 1 == upper_bound->value) {
      continue;
    }
    // If both is not the case, we can reduce the valuations
    tchecker::dbm::reset_to_value(zone->dbm(), zone->dim(), i, upper_bound->value - (tchecker::LE == upper_bound->cmp) ? 0 : 1);
  }
}

std::shared_ptr<tchecker::clockval_t> convert(const tchecker::zg::zone_t & zone)
{
  std::shared_ptr<tchecker::zg::zone_t> copy = tchecker::zg::factory(zone);

  compress_zone(copy);

  auto result = clockval_factory(copy->dim());

  for(std::size_t i = 0; i < copy->dim(); i++) {
    tchecker::clock_rational_value_t value;
    auto upper_bound = tchecker::dbm::access(copy->dbm(), copy->dim(), i, 0);
    if(upper_bound->cmp == tchecker::LT) {
      value = tchecker::clock_rational_value_t(2*(upper_bound->value - 1) + 1, 2);
    } else {
      value = tchecker::clock_rational_value_t(upper_bound->value, 1);
    }
    (*result)[i] = value;
  }

  if(!zone.belongs(*result)) {
    std::string err = std::string("The zone\n");

    std::stringstream sstream;
    tchecker::dbm::output_matrix(sstream, zone.dbm(), zone.dim());
    
    err = err + sstream.str() + "\n" + "could not be converted into a single valuation";
    throw std::runtime_error(err);
  }
  
  return result;
}

std::shared_ptr<tchecker::zg::zone_t> convert(std::shared_ptr<tchecker::clockval_t> clockval)
{
  std::shared_ptr<tchecker::zg::zone_t> result = tchecker::zg::factory(clockval->size());
  result->make_universal();

  result->make_region(clockval);

  return result;

}

} // end of namespace operational_semantics

} // end of namespace tchecker