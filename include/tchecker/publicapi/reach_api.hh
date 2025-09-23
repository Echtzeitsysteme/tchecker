/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

/*!
  \file reach_api.hh
  \brief API for reachability checking on TChecker system declarations
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
/*!
  \brief Checks reachability properties on a TChecker system declaration

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It checks reachability properties on the system described in \p sysdecl_filename using the specified \p algorithm,
  and writes the result or certificate to \p output_filename.

  \param output_filename Path to the output file (results or certificate)
  \param sysdecl_filename Path to the system declaration file
  \param labels String describing the labels for reachability checking
  \param algorithm Reachability checking algorithm (see tck_reach_algorithm_t)
  \param search_order Search order for state-space exploration ("BFS" or "DFS")
  \param certificate Type of certificate to produce (see tck_reach_certificate_t)
  \param block_size Pointer to the block size for internal computation (nullptr for default)
  \param table_size Pointer to the table size for internal computation (nullptr for default)

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  # Example usage from Python with ctypes
  import ctypes

  # Load the shared library (adjust path as needed)
  lib = ctypes.CDLL("libtchecker.so")

  # Define argument types
  lib.tck_reach.argtypes = [
      ctypes.c_char_p,  # output_filename
      ctypes.c_char_p,  # sysdecl_filename
      ctypes.c_char_p,  # labels
      ctypes.c_int,     # tck_reach_algorithm_t
      ctypes.c_char_p,  # search_order ("BFS" or "DFS")
      ctypes.c_int,     # tck_reach_certificate_t
      ctypes.POINTER(ctypes.c_int),  # block_size
      ctypes.POINTER(ctypes.c_int)   # table_size
  ]

  # Example call
  output_filename = b"reach.txt"
  sysdecl_filename = b"system.tck"
  labels = b"target"
  algorithm = 0    # ALGO_REACH
  search_order = b"BFS"
  certificate = 0  # CERTIFICATE_GRAPH
  block_size = ctypes.c_int(10000)
  table_size = ctypes.c_int(65536)

  lib.tck_reach(
      output_filename,
      sysdecl_filename,
      labels,
      algorithm,
      search_order,
      certificate,
      ctypes.byref(block_size),
      ctypes.byref(table_size)
  )
  \endcode
*/
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
  \brief Checks reachability properties on a TChecker system declaration

  This function checks reachability properties on the system described in \p sysdecl_filename using the specified \p algorithm,
  and writes the result or certificate to \p output_filename.

  \param output_filename Path to the output file (results or certificate)
  \param sysdecl_filename Path to the system declaration file
  \param labels String describing the labels for reachability checking
  \param algorithm Reachability checking algorithm (see tck_reach_algorithm_t)
  \param search_order Search order for state-space exploration ("BFS" or "DFS")
  \param certificate Type of certificate to produce (see tck_reach_certificate_t)
  \param block_size Block size for internal computation
  \param table_size Table size for internal computation

  \note This is the C++ API. For C/FFI usage, see the C-compatible version above.
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