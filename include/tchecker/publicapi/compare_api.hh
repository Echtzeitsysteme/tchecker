/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

  /*!
    \file compare_api.hh
    \brief API for comparing two TChecker system declarations
  */

#ifndef TCHECKER_PUBLICAPI_COMPARE_API_HH
#define TCHECKER_PUBLICAPI_COMPARE_API_HH

#include <string>

#define TCK_COMPARE_INIT_BLOCK_SIZE 10000;
#define TCK_COMPARE_INIT_TABLE_SIZE 65536;

/*!
  \brief Type of relationship for comparison
*/
enum tck_compare_relationship_t {
  STRONG_TIMED_BISIM, /*!< Strong Timed Bisimilarity */
};

#ifdef __cplusplus
extern "C" {
#endif


/*!
  \brief Compares two TChecker system declarations

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It compares the systems described in \p first_sysdecl_filename and \p second_sysdecl_filename
  according to the specified \p relationship, and writes the result to \p output_filename.

  \param output_filename Path to the output file (comparison results)
  \param first_sysdecl_filename Path to the first system declaration file
  \param second_sysdecl_filename Path to the second system declaration file
  \param relationship Type of relationship to check (see tck_compare_relationship_t)
  \param block_size Pointer to the block size for internal computation (nullptr for default)
  \param table_size Pointer to the table size for internal computation (nullptr for default)

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  # Example usage from Python with ctypes
  import ctypes

  # Load the shared library (adjust path as needed)
  lib = ctypes.CDLL("libtchecker.so")

  # Define argument types
  lib.tck_compare.argtypes = [
      ctypes.c_char_p,  # output_filename
      ctypes.c_char_p,  # first_sysdecl_filename
      ctypes.c_char_p,  # second_sysdecl_filename
      ctypes.c_int,     # tck_compare_relationship_t
      ctypes.POINTER(ctypes.c_int),  # block_size
      ctypes.POINTER(ctypes.c_int)   # table_size
  ]

  # Example call
  output_filename = b"compare.txt"
  first_sysdecl_filename = b"system1.tck"
  second_sysdecl_filename = b"system2.tck"
  relationship = 0  # STRONG_TIMED_BISIM
  block_size = ctypes.c_int(10000)
  table_size = ctypes.c_int(65536)

  lib.tck_compare(
      output_filename,
      first_sysdecl_filename,
      second_sysdecl_filename,
      relationship,
      ctypes.byref(block_size),
      ctypes.byref(table_size)
  )
  \endcode
*/
void tck_compare(const char * output_filename, 
  const char * first_sysdecl_filename, 
  const char * second_sysdecl_filename,
  tck_compare_relationship_t relationship,
  int * block_size, 
  int * table_size,
  const char * starting_state_attributes_first = nullptr,
  const char * starting_state_attributes_second = nullptr,
  bool generate_witness=false);

#ifdef __cplusplus
}
#endif


namespace tchecker {

namespace publicapi {


  /*!
  \brief Compares two TChecker system declarations

  This function compares the systems described in \p first_sysdecl_filename and \p second_sysdecl_filename
  according to the specified \p relationship, and writes the result to \p output_filename.

  \param output_filename Path to the output file (comparison results)
  \param first_sysdecl_filename Path to the first system declaration file
  \param second_sysdecl_filename Path to the second system declaration file
  \param relationship Type of relationship to check (see tck_compare_relationship_t)
  \param block_size Block size for internal computation
  \param table_size Table size for internal computation
  \param first_stating_state_json the json description of the first starting state, or the empty string in case init shall be used
  \param second_stating_state_json the json description of the second starting state, or the empty string in case init shall be used
  \param generate_witness Whether a witness/Contradiction DAG shall be generated

  \note This is the C++ API. For C/FFI usage, see the C-compatible version above.
*/
  void tck_compare(std::string output_filename, 
                   std::string first_sysdecl_filename, 
                   std::string second_sysdecl_filename,
                   tck_compare_relationship_t relationship,
                   std::size_t block_size, 
                   std::size_t table_size,
                   std::string first_starting_state_json,
                   std::string second_starting_state_json,
                   bool generate_witness);

} // end of namespace publicapi

} // end of namespace tchecker


#endif