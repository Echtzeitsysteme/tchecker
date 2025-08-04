/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PUBLICAPI_SIMULATE_API_HH
#define TCHECKER_PUBLICAPI_SIMULATE_API_HH


#include <tchecker/simulate/simulate.hh>

#ifdef __cplusplus
extern "C" {
#endif

const void tck_simulate_onestep_simulation(const char * output_filename, const char * sysdecl_filename,
                                           const tchecker::simulate::display_type_t display_type,
                                           const char * starting_state_attributes);

#ifdef __cplusplus
}
#endif

#endif