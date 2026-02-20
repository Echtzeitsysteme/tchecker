/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/operational-semantics/attributes_to_valuation.hh"

#include "tchecker/utils/string.hh"

namespace tchecker {

namespace operational_semantics {

std::shared_ptr<tchecker::clockval_t> build(std::map<std::string, std::string> const & attributes)
{
  std::vector<std::string> str_assignments = tchecker::split(attributes.at("clockval"), ',');

  auto raw = clockval_allocate_and_construct(str_assignments.size() + 1); // +1 due to refclock
  auto result = std::shared_ptr<tchecker::clockval_t>(raw, &clockval_destruct_and_deallocate);

  (*result)[0] = 0;
  for(std::size_t i = 1; i <=str_assignments.size(); ++i) {
    std::vector<std::string> assignment = tchecker::split(str_assignments[i-1], '=');
    std::vector<std::string> quotient = tchecker::split(assignment[1], '/');

    long int num, den;
    num = std::stol(quotient[0]);

    if(quotient.size() == 1) {
      den = 1;
    } else if (quotient.size() == 2) {
      den = std::stol(quotient[1]);
    } else {
      throw std::runtime_error("Invalid Clockval Format" + assignment[i-1]);
    }

    if(num > INT64_MAX || den > INT64_MAX) {
      throw std::runtime_error("Invalid Clockval Format" + assignment[i-1]);
    }


    (*result)[i] = tchecker::clock_rational_value_t(static_cast<int64_t>(num), static_cast<int64_t>(den));
  }
  
  return result;

}

} // end of namespace operational_semantics

} // end of namespace tchecker