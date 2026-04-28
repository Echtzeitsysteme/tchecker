/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/operational-semantics/calculate_transitions.hh"

namespace tchecker {

namespace operational_semantics {

std::shared_ptr<tchecker::clockval_t> next(tchecker::clockval_t & clockval, tchecker::clock_reset_container_t const & clkreset)
{
  auto result = tchecker::clockval_factory(clockval);
  for (tchecker::clock_reset_t const & r : clkreset) {
    if(tchecker::REFCLOCK_ID == r.right_id()) {
      (*result)[r.left_id() + 1] = r.value();
    } else {
      (*result)[r.left_id() + 1] = (*result)[r.right_id() + 1] + r.value();
    }
  }
  return result;
}


} // end of namespace operational_semantics

} // end of namespace tchecker