/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_SYMBOLIC_SYMBOLIC_SIMULATE_HH
#define TCHECKER_TCK_SIMULATE_SYMBOLIC_SYMBOLIC_SIMULATE_HH

/*!
 \file Symbolic simulate.hh
 \brief Symbolic simulation of timed automata
*/

#include <map>
#include <memory>
#include <string>

#include "symbolic_display.hh"
#include "symbolic_graph.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace simulate {

namespace symbolic {

static std::size_t const NO_SELECTION = std::numeric_limits<std::size_t>::max();


/*!
 \brief Randomized simulation of timed automata
 \param sysdecl : system declaration
 \param nsteps : number of simulation steps
 \param starting_state_attributes : attributes of simulation starting state
 \return state-space consisting of a zone graph and the simulation tree built from nsteps randomized
 simulation steps of the system of timed processes sysdecl
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
std::shared_ptr<tchecker::simulate::symbolic::state_space_t>
randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl, 
                      enum tchecker::simulate::display_type_t display_type, 
                      std::ostream & os,
                      std::map<std::string, std::string> const & starting_state_attributes,
                      std::size_t nsteps);

/*!
 \brief Interactive simulation of timed automata
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \return state-space consisting of a zone graph and the simulation tree built from interactive
 simulation of the system of timed processes sysdecl
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
std::shared_ptr<tchecker::simulate::symbolic::state_space_t>
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

} // end of namespace symbolic                        

} // namespace simulate

} // namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_SIMULATE_HH
