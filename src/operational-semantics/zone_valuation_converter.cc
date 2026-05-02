/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/operational-semantics/zone_valuation_converter.hh"

namespace tchecker {

namespace operational_semantics {

tchecker::dbm::status_t compress_zone(std::shared_ptr<tchecker::zg::zone_t> zone)
{
  for(tchecker::clock_id_t i = 1; i < zone->dim(); i++) {
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
    tchecker::ineq_cmp_t cmp = tchecker::LE;
    tchecker::integer_t value = upper_bound->value;
    if(tchecker::LT == upper_bound->cmp) {
      value--;
      cmp = tchecker::LT;
    }
    value *=-1;
    if(tchecker::dbm::status_t::EMPTY == tchecker::dbm::constrain(zone->dbm(), zone->dim(), 0, i, cmp, value)) {
      return tchecker::dbm::status_t::EMPTY;
    }
  }

  assert(!tchecker::dbm::is_empty_0(zone->dbm(), zone->dim()));
  return tchecker::dbm::status_t::NON_EMPTY;
}

bool is_compressed(const tchecker::zg::zone_t & zone) {
  bool result = true;
  for(tchecker::clock_id_t i = 1; i < zone.dim(); i++) {
    auto upper_bound = tchecker::dbm::access(zone.dbm(), zone.dim(), i, 0);
    auto lower_bound = tchecker::dbm::access(zone.dbm(), zone.dim(), 0, i);
    bool inter = tchecker::LE == upper_bound->cmp && tchecker::LE == lower_bound->cmp && (-1)*lower_bound->value == upper_bound->value;
    inter |= tchecker::LT == upper_bound->cmp && tchecker::LT == lower_bound->cmp && (-1)*lower_bound->value + 1 == upper_bound->value;
    result &= inter;
  }
  return result;
}

void create_next_clockval_value(const tchecker::zg::zone_t & zone, std::shared_ptr<tchecker::clockval_t> clockval, 
                                    std::vector<tchecker::clock_id_t> & v_LTs, tchecker::clock_rational_value_t step)
{
  if(v_LTs.empty()) {
    return;
  }
  std::vector<tchecker::clock_id_t> copy_LT(v_LTs); 
  tchecker::clock_id_t myID = copy_LT.back();
  copy_LT.pop_back();
  auto upper_bound = tchecker::dbm::access(zone.dbm(), zone.dim(), myID, 0);
  tchecker::clock_rational_value_t starting_val = (*clockval)[myID];
  while(!zone.belongs(*clockval) && ((*clockval)[myID] + step) < upper_bound->value) {
    (*clockval)[myID] = (*clockval)[myID] + step;
    create_next_clockval_value(zone, clockval, copy_LT, step);
  }

  if(!zone.belongs(*clockval)) {
    (*clockval)[myID] = starting_val;
  }
}

std::shared_ptr<tchecker::clockval_t> create_clockval(const tchecker::zg::zone_t & zone, tchecker::clock_rational_value_t step)
{
  assert(is_compressed(zone));

  std::shared_ptr<tchecker::clockval_t> result = tchecker::clockval_factory(zone.dim());
  std::vector<tchecker::clock_id_t> v_LTs;

  for(tchecker::clock_id_t i = 0; i < result->size(); i++) {
    tchecker::clock_rational_value_t value;
    auto upper_bound = tchecker::dbm::access(zone.dbm(), zone.dim(), i, 0);
    if(upper_bound->cmp == tchecker::LT) {
      v_LTs.insert(v_LTs.begin(), i); // This must be done at the beginning!
      value = tchecker::clock_rational_value_t(upper_bound->value - 1, 1) + step;
    } else {
      value = tchecker::clock_rational_value_t(upper_bound->value, 1);
    }
    (*result)[i] = value;
  }

  create_next_clockval_value(zone, result, v_LTs, step);

  return result;
}

std::shared_ptr<tchecker::clockval_t> convert(const tchecker::zg::zone_t & zone)
{
  assert(!tchecker::dbm::is_empty_0(zone.dbm(), zone.dim()));
  std::shared_ptr<tchecker::zg::zone_t> copy = tchecker::zg::factory(zone);

  compress_zone(copy);

  tchecker::clock_rational_value_t step(1, 2);
  std::shared_ptr<tchecker::clockval_t> result = create_clockval(*copy, step);
  while(!zone.belongs(*result)){
    step = step/2;
    result = create_clockval(*copy, step);
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