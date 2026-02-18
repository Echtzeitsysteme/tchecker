/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/simulate.hh"
#include "tchecker/simulate/symbolic/symbolic_simulate.hh"
#include "tchecker/simulate/concrete/concrete_simulate.hh"

namespace tchecker {

namespace simulate {

std::shared_ptr<tchecker::simulate::state_space_t>
randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl, std::size_t nsteps,
                      std::map<std::string, std::string> const & starting_state_attributes) {
  return tchecker::simulate::symbolic::randomized_simulation(sysdecl, nsteps, starting_state_attributes);
}

std::size_t randomized_select(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  assert(v.size() < NO_SELECTION);
  if (v.size() == 0)
    return NO_SELECTION;
  return std::rand() % v.size();
}


std::shared_ptr<tchecker::simulate::state_space_t>
interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                       enum tchecker::simulate::display_type_t display_type,
                       std::ostream & os,
                       std::map<std::string, std::string> const & starting_state_attributes) {
  return tchecker::simulate::symbolic::interactive_simulation(sysdecl, display_type, os, starting_state_attributes);
}

void onestep_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os,
                        std::map<std::string, std::string> const & starting_state_attributes) {
  return tchecker::simulate::symbolic::onestep_simulation(sysdecl, display_type, os, starting_state_attributes);
}

std::shared_ptr<tchecker::simulate::state_space_t>
concrete_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os) {
  return tchecker::simulate::concrete::interactive_simulation(sysdecl, display_type, os);
}

} // end of namespace simulate

} // end of namespace tchecker