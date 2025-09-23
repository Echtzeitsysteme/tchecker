/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

/*!
  \file liveness_api.hh
  \brief API for liveness checking on TChecker system declarations
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
  \brief Checks liveness properties on a TChecker system declaration

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It checks liveness properties on the system described in \p sysdecl_filename using the specified \p algorithm,
  and writes the result or certificate to \p output_filename.

  \param output_filename Path to the output file (results or certificate)
  \param sysdecl_filename Path to the system declaration file
  \param labels String describing the labels for liveness checking
  \param algorithm Liveness checking algorithm (see tck_liveness_algorithm_t)
  \param certificate Type of certificate to produce (see tck_liveness_certificate_t)
  \param block_size Pointer to the block size for internal computation (nullptr for default)
  \param table_size Pointer to the table size for internal computation (nullptr for default)

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  # Example usage from Python with ctypes
  import ctypes

  # Load the shared library (adjust path as needed)
  lib = ctypes.CDLL("libtchecker.so")

  # Define argument types
  lib.tck_liveness.argtypes = [
      ctypes.c_char_p,  # output_filename
      ctypes.c_char_p,  # sysdecl_filename
      ctypes.c_char_p,  # labels
      ctypes.c_int,     # tck_liveness_algorithm_t
      ctypes.c_int,     # tck_liveness_certificate_t
      ctypes.POINTER(ctypes.c_int),  # block_size
      ctypes.POINTER(ctypes.c_int)   # table_size
  ]

  # Example call
  output_filename = b"liveness.txt"
  sysdecl_filename = b"system.tck"
  labels = b"accepting"
  algorithm = 0    # ALGO_COUVSCC
  certificate = 0  # CERTIFICATE_GRAPH
  block_size = ctypes.c_int(10000)
  table_size = ctypes.c_int(65536)

  lib.tck_liveness(
      output_filename,
      sysdecl_filename,
      labels,
      algorithm,
      certificate,
      ctypes.byref(block_size),
      ctypes.byref(table_size)
  )
  \endcode
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
  \brief Checks liveness properties on a TChecker system declaration

  This function checks liveness properties on the system described in \p sysdecl_filename using the specified \p algorithm,
  and writes the result or certificate to \p output_filename.

  \param output_filename Path to the output file (results or certificate)
  \param sysdecl_filename Path to the system declaration file
  \param labels String describing the labels for liveness checking
  \param algorithm Liveness checking algorithm (see tck_liveness_algorithm_t)
  \param certificate Type of certificate to produce (see tck_liveness_certificate_t)
  \param block_size Block size for internal computation
  \param table_size Table size for internal computation

  \note This is the C++ API. For C/FFI usage, see the C-compatible version above.
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

