/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PUBLICAPI_LIVENESS_API_HH
#define TCHECKER_PUBLICAPI_LIVENESS_API_HH

#include <string>


#define TCK_LIVENESS_INIT_BLOCK_SIZE 10000;
#define TCK_LIVENESS_INIT_TABLE_SIZE 65536;

enum tck_liveness_algorithm_t {
  ALGO_COUVSCC, /*!< Couvreur's SCC algorithm */
  ALGO_NDFS,    /*!< Nested DFS algorithm */
  ALGO_NONE,    /*!< No algorithm */
};

enum tck_liveness_certificate_t {
  CERTIFICATE_GRAPH,    /*!< Graph of state-space */
  CERTIFICATE_SYMBOLIC, /*!< Symbolic counter-example */
  CERTIFICATE_NONE,     /*!< No certificate */
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
   \brief C++ function to be called from python TODO: doc
  */
void tck_liveness(const char * output_filename, 
    const char * sysdecl_filename, 
    const char * labels, 
    tck_liveness_algorithm_t algorithm, 
    tck_liveness_certificate_t 
    certificate, 
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
  void tck_liveness(std::string output_filename, 
                   std::string sysdecl_filename, 
                   std::string labels, 
                   tck_liveness_algorithm_t algorithm, 
                   tck_liveness_certificate_t certificate, 
                   std::size_t block_size, 
                   std::size_t table_size);
} // end of namespace publicapi

} // end of namespace tchecker

#endif

