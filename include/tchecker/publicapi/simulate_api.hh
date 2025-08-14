/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PUBLICAPI_SIMULATE_API_HH
#define TCHECKER_PUBLICAPI_SIMULATE_API_HH

#include <tchecker/simulate/simulate.hh>

/*!
 \brief Type of simulation
*/
enum simulation_type_t {
  INTERACTIVE_SIMULATION, /*!< Interactive simulation */
  ONESTEP_SIMULATION,     /*!< One-step simulation */
  RANDOMIZED_SIMULATION,  /*!< Randomized simulation */
};

#ifdef __cplusplus
extern "C" {
#endif

void tck_simulate(const char * output_filename, const char * sysdecl_filename, simulation_type_t simulation_type,
                  const tchecker::simulate::display_type_t display_type, const char * starting_state_attributes, int nsteps,
                  bool output_trace);

#ifdef __cplusplus
}
#endif

namespace tchecker {

namespace publicapi {

void tck_simulate(std::string output_filename, std::string sysdecl_filename, simulation_type_t simulation_type,
                  tchecker::simulate::display_type_t display_type, std::string starting_state_attributes,
                  std::size_t nsteps, bool output_trace);

} // end of namespace publicapi

} // end of namespace tchecker

#endif