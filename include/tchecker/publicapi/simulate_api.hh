/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */


 /*!
  \file simulate_api.hh
  \brief API for simulating TChecker system declarations
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

/*!
  \brief Runs a simulation on a TChecker system declaration

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It simulates the system described in \p sysdecl_filename and writes output to \p output_filename.

  \param output_filename Path to the output file (trace or results)
  \param sysdecl_filename Path to the system declaration file
  \param simulation_type Type of simulation (see simulation_type_t)
  \param display_type Type of display for simulation output (see tchecker::simulate::display_type_t)
  \param starting_state_attributes String describing the starting state attributes
  \param nsteps Pointer to the number of steps to simulate (nullptr for unlimited)
  \param output_trace If true, output the simulation trace

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  # Example usage from Python with ctypes
  import ctypes

  # Load the shared library (adjust path as needed)
  lib = ctypes.CDLL("libtchecker.so")

  # Define argument types
  lib.tck_simulate.argtypes = [
      ctypes.c_char_p,  # output_filename
      ctypes.c_char_p,  # sysdecl_filename
      ctypes.c_int,     # simulation_type_t
      ctypes.c_int,     # display_type_t
      ctypes.c_char_p,  # starting_state_attributes
      ctypes.POINTER(ctypes.c_int),  # nsteps
      ctypes.c_bool     # output_trace
  ]

  # Example call
  output_filename = b"output.txt"
  sysdecl_filename = b"system.tck"
  simulation_type = 0  # INTERACTIVE_SIMULATION
  display_type = 0     # e.g., DISPLAY_NONE
  starting_state_attributes = b"init"
  nsteps = ctypes.c_int(10)
  output_trace = True

  lib.tck_simulate(
      output_filename,
      sysdecl_filename,
      simulation_type,
      display_type,
      starting_state_attributes,
      ctypes.byref(nsteps),
      output_trace
  )
  \endcode
*/
void tck_simulate(const char * output_filename, const char * sysdecl_filename, const simulation_type_t simulation_type,
                  const tchecker::simulate::display_type_t display_type, const char * starting_state_attributes,
                  const int * nsteps, const bool output_trace);

#ifdef __cplusplus
}
#endif

namespace tchecker {

namespace publicapi {

/*!
  \brief Runs a simulation on a TChecker system declaration

  This function simulates the system described in \p sysdecl_filename and writes output to \p output_filename.

  \param output_filename Path to the output file (trace or results)
  \param sysdecl_filename Path to the system declaration file
  \param simulation_type Type of simulation (see simulation_type_t)
  \param display_type Type of display for simulation output (see tchecker::simulate::display_type_t)
  \param starting_state_attributes String describing the starting state attributes
  \param nsteps Number of steps to simulate (use std::numeric_limits<std::size_t>::max() for unlimited)
  \param output_trace If true, output the simulation trace

  \note This is the C++ API. For C/FFI usage, see the C-compatible version above.
*/
void tck_simulate(std::string output_filename, std::string sysdecl_filename, simulation_type_t simulation_type,
                  tchecker::simulate::display_type_t display_type, std::string starting_state_attributes, std::size_t nsteps,
                  bool output_trace);


} // end of namespace publicapi

} // end of namespace tchecker

#endif