/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_CONCRETE_CONCRETE_SIMULATE_HH
#define TCHECKER_TCK_SIMULATE_CONCRETE_CONCRETE_SIMULATE_HH

/*!
 \file concrete_simulate.hh
 \brief Concrete simulation of timed automata
*/

#include <map>
#include <memory>
#include <string>

#include "tchecker/simulate/simulate.hh"
#include "tchecker/simulate/concrete/concrete_display.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace simulate {

namespace concrete {

/*!
 \brief One-step simulation of timed automata using concrete values
 \param sysdecl : system declaration
 \param display_type : type of display
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
std::shared_ptr<tchecker::simulate::state_space_t>
interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os);

/*!
 \brief Concrete one-step simulation of timed automata
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
void onestep_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os,
                        std::map<std::string, std::string> const & starting_state_attributes);

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_SIMULATE_HH
