/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PUBLICAPI_COMPARE_API_HH
#define TCHECKER_PUBLICAPI_COMPARE_API_HH

#include <string>

#define TCK_COMPARE_INIT_BLOCK_SIZE 10000;
#define TCK_COMPARE_INIT_TABLE_SIZE 65536;

enum tck_compare_relationship_t {
  STRONG_TIMED_BISIM, /*!< Strong Timed Bisimilarity */
};

#ifdef __cplusplus
extern "C" {
#endif

void tck_compare(const char * output_filename, 
  const char * first_sysdecl_filename, 
  const char * second_sysdecl_filename,
  tck_compare_relationship_t relationship,
  int * block_size, 
  int * table_size);

#ifdef __cplusplus
}
#endif


namespace tchecker {

namespace publicapi {

  void tck_compare(std::string output_filename, 
                  std::string first_sysdecl_filename, 
                  std::string second_sysdecl_filename,
                  tck_compare_relationship_t relationship,
                  std::size_t block_size, 
                  std::size_t table_size);

} // end of namespace publicapi

} // end of namespace tchecker


#endif