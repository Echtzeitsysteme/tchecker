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
#include "tchecker/simulate/concrete/concrete_graph.hh"
#include "tchecker/simulate/concrete/concrete_display.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace simulate {

namespace concrete {


/*!
 \brief Concrete Transition Types
*/
enum concrete_trans_type_t {
  ACTION = 0,
  DELAY,
  ERROR
};


/*!
 \class concrete_simulator_t
 \brief The concrete simulator
 */
class concrete_simulator_t {
 public:
  /*!
  \brief Constructor
  \param sysdecl : system declaration
  \param display_type : type of display
  \param os : the output stream
  */
  concrete_simulator_t(tchecker::parsing::system_declaration_t const & sysdecl,
                       enum tchecker::simulate::display_type_t display_type,
                       std::ostream & os);

  /*!
  \brief One-step simulation of timed automata using concrete values
  \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
  */
  std::shared_ptr<tchecker::simulate::concrete::state_space_t>
  interactive_simulation();

 /*!
  \brief Concrete one-step simulation of timed automata
  \param starting_state_attributes : attributes of simulation starting state
  \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
  */
  void onestep_simulation(std::map<std::string, std::string> const & starting_state_attributes);

  /*!
  \brief Concrete randomized simulation of timed automata
  \param sysdecl : system declaration
  \param display_type : type of display
  \param starting_state_attributes : attributes of simulation starting state
  \param nsteps : number of simulation steps
  \return state-space consisting of a zone graph and the simulation tree built from nsteps randomized
  simulation steps of the system of timed processes sysdecl
  \note simulation starts from the initial state of sysdecl if starting_state_attributes is empty
  */
  std::shared_ptr<tchecker::simulate::concrete::state_space_t>
  randomized_simulation(std::map<std::string, std::string> const & starting_state_attributes, std::size_t nsteps);

 private:
  
  std::shared_ptr<tchecker::ta::system_t const> _system;
  std::shared_ptr<tchecker::zg::zg_t> _zg;
  std::shared_ptr<state_space_t> _state_space;
  graph_t & _g;
  std::vector<tchecker::zg::zg_t::sst_t> _v;
  std::unique_ptr<concrete_display_t> _display;
  tchecker::clockbounds::bound_t _highest_delay;
  std::ostream & _os;
  tchecker::zg::state_sptr_t _previous_symb;
  std::shared_ptr<tchecker::clock_constraint_container_t> _previous_node_inv;
  std::shared_ptr<node_t> _previous_node;

  std::pair<concrete_trans_type_t, tchecker::clock_rational_value_t> 
  concrete_interactive_select(std::ostream & s_out,
                              std::function<std::string()> input_func, 
                              concrete_display_t & display, 
                              tchecker::zg::const_state_sptr_t const & s,
                              std::vector<tchecker::zg::zg_t::sst_t> const & v,
                              bool finite_max_delay,
                              tchecker::clock_rational_value_t max_delay,
                              bool print_interaction);

  std::pair<tchecker::simulate::concrete::concrete_trans_type_t, tchecker::clock_rational_value_t>
  initial_select(std::function<std::string()> input_func, bool print_interaction);

  std::pair<tchecker::simulate::concrete::concrete_trans_type_t, tchecker::clock_rational_value_t>
  next_select(std::function<std::string()> input_func, bool print_interaction);

  tchecker::clock_rational_value_t calculate_max_delay();

  int parse_starting_state_attributes(std::map<std::string, std::string> const & starting_state_attributes);

};

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_SIMULATE_HH
