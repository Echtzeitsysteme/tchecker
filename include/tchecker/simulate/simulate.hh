/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_SIMULATE_HH
#define TCHECKER_TCK_SIMULATE_SIMULATE_HH

/*!
 \file simulate.hh
 \brief Simulation of timed automata
*/

#include <map>
#include <memory>
#include <string>

#include "display.hh"
#include "graph.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace simulate {

/*!
 \brief Randomized simulation of timed automata
 \param sysdecl : system declaration
 \param nsteps : number of simulation steps
 \param starting_state_attributes : attributes of simulation starting state
 \return state-space consisting of a zone graph and the simulation tree built from nsteps randomized
 simulation steps of the system of timed processes sysdecl
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/

std::shared_ptr<tchecker::simulate::state_space_t>
randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl, std::size_t nsteps,
                      std::map<std::string, std::string> const & starting_state_attributes);

/*!
 \brief Interactive simulation of timed automata
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \return state-space consisting of a zone graph and the simulation tree built from interactive
 simulation of the system of timed processes sysdecl
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
std::shared_ptr<tchecker::simulate::state_space_t>
interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                       enum tchecker::simulate::display_type_t display_type,
                       std::ostream & os,
                       std::map<std::string, std::string> const & starting_state_attributes);

/*!
 \brief One-step simulation of timed automata
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
void onestep_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                        enum tchecker::simulate::display_type_t display_type,
                        std::ostream & os,
                        std::map<std::string, std::string> const & starting_state_attributes);

} // namespace simulate

} // namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_SIMULATE_HH
