/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PUBLICAPI_REACH_API_HH
#define TCHECKER_PUBLICAPI_REACH_API_HH

#include <string>

#define TCK_REACH_INIT_BLOCK_SIZE 10000;
#define TCK_REACH_INIT_TABLE_SIZE 65536;

enum tck_reach_algorithm_t {
  ALGO_REACH,        /*!< Reachability algorithm */
  ALGO_CONCUR19,     /*!< Covering reachability algorithm over the local-time zone graph */
  ALGO_COVREACH,     /*!< Covering reachability algorithm */
  ALGO_ALU_COVREACH, /*!< Covering reachability algorithm with aLU subsumption*/
  ALGO_NONE,         /*!< No algorithm */
};


enum tck_reach_certificate_t {
  CERTIFICATE_GRAPH,    /*!< Graph of state-space */
  CERTIFICATE_SYMBOLIC, /*!< Symbolic counter-example */
  CERTIFICATE_CONCRETE, /*!< Concrete counter-example */
  CERTIFICATE_NONE,     /*!< No certificate */
};

enum tck_reach_search_order_t {
  BFS,
  DFS
};

#ifdef __cplusplus
extern "C" {
#endif
/* function to be called from python TODO: doc */
void tck_reach(const char * output_filename, 
  const char * sysdecl_filename, 
  const char * labels, 
  tck_reach_algorithm_t algorithm, 
  const char * search_order, 
  tck_reach_certificate_t certificate, 
  int* block_size, 
  int* table_size);

#ifdef __cplusplus
}
#endif

namespace tchecker {

namespace publicapi {

  /*!
   * TODO: doc
   */
  void tck_reach(std::string output_filename, 
                 std::string sysdecl_filename, 
                 std::string labels, 
                 tck_reach_algorithm_t algorithm, 
                 std::string search_order, 
                 tck_reach_certificate_t certificate, 
                 std::size_t block_size, 
                 std::size_t table_size);

} // end of namespace publicapi

} // end of namespace tchecker

#endif