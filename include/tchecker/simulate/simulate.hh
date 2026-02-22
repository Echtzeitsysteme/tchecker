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

#include "tchecker/parsing/parsing.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace simulate {

/*!
 \brief Type of display
*/
enum display_type_t {
  HUMAN_READABLE_DISPLAY = 0, /*!< Human readable display */
#if USE_BOOST_JSON
  JSON_DISPLAY,               /*!< JSON display */
#endif
};

/*!
 \class state_space_t
 \brief an interface of the return type of any simulate function
 */
class state_space_t {

 public:
  /*!
   \brief Graph output
   \param os : output stream
   \param name : graph name
   \post this with name has been output to os
  */
  virtual void dot_output(std::ostream & os, std::string const & name) = 0;
};

/*!
 \brief Randomized simulation of timed automata
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \param nsteps : number of simulation steps
 \return state-space consisting of a zone graph and the simulation tree built from nsteps randomized
 simulation steps of the system of timed processes sysdecl
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/

std::shared_ptr<tchecker::simulate::state_space_t>
randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl, 
                      enum tchecker::simulate::display_type_t display_type, 
                      std::ostream & os,
                      std::map<std::string, std::string> const & starting_state_attributes,
                      std::size_t nsteps);

/*!
 \brief Random selection
 \param v : a vector of triples (status, state, transition)
 \pre the size of v is less than NO_SELECTION (checked by assertion)
 \return the index of the chosen element in v if v is not empty,
 tchecker::simulate::NO_SELECTION otherwise
*/
std::size_t randomized_select(std::vector<tchecker::zg::zg_t::sst_t> const & v);

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

/*!
 \brief Interactive simulation of timed automata using concrete values
 \param sysdecl : system declaration
 \param display_type : type of display
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
std::shared_ptr<tchecker::simulate::state_space_t>
concrete_interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                               enum tchecker::simulate::display_type_t display_type,
                               std::ostream & os);

/*!
 \brief One-step simulation of timed automata
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
void concrete_onestep_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                                enum tchecker::simulate::display_type_t display_type,
                                std::ostream & os,
                                std::map<std::string, std::string> const & starting_state_attributes);

/*!
 \brief Randomized simulation of timed automata using concrete values
 \param sysdecl : system declaration
 \param display_type : type of display
 \param starting_state_attributes : attributes of simulation starting state
 \param nsteps : number of simulation steps
 \return state-space consisting of a zone graph and the simulation tree built from nsteps randomized
 simulation steps of the system of timed processes sysdecl
 \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
*/
std::shared_ptr<tchecker::simulate::state_space_t>
concrete_randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl, 
                               enum tchecker::simulate::display_type_t display_type, 
                               std::ostream & os,
                               std::map<std::string, std::string> const & starting_state_attributes,
                               std::size_t nsteps);

} // namespace simulate

} // namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_SIMULATE_HH