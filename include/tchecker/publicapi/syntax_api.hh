/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

/*!
  \file syntax_api.hh
  \brief API for syntax checking and system declaration manipulation
*/

#ifndef TCHECKER_PUBLICAPI_SYNTAX_API_HH
#define TCHECKER_PUBLICAPI_SYNTAX_API_HH

#include <string>

#ifdef __cplusplus
extern "C" {    
#endif

/*!
  \brief Checks the syntax of a TChecker system declaration

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It checks the syntax of the system described in \p sysdecl_filename and writes the result to \p output_filename.

  \param output_filename Path to the output file (syntax check results)
  \param sysdecl_filename Path to the system declaration file

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  import ctypes
  lib = ctypes.CDLL("libtchecker.so")
  lib.tck_syntax_check_syntax.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
  output_filename = b"syntax.txt"
  sysdecl_filename = b"system.tck"
  lib.tck_syntax_check_syntax(output_filename, sysdecl_filename)
  \endcode
*/
void tck_syntax_check_syntax(const char * output_filename, const char * sysdecl_filename);

/*!
  \brief Outputs a TChecker system declaration in DOT (Graphviz) format

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It outputs the system described in \p sysdecl_filename to \p output_filename in DOT format.

  \param output_filename Path to the output file (DOT format)
  \param sysdecl_filename Path to the system declaration file

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  import ctypes
  lib = ctypes.CDLL("libtchecker.so")
  lib.tck_syntax_to_dot.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
  output_filename = b"system.dot"
  sysdecl_filename = b"system.tck"
  lib.tck_syntax_to_dot(output_filename, sysdecl_filename)
  \endcode
*/
void tck_syntax_to_dot(const char * output_filename, const char * sysdecl_filename);

/*!
  \brief Outputs a TChecker system declaration in JSON format

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It outputs the system described in \p sysdecl_filename to \p output_filename in JSON format.

  \param output_filename Path to the output file (JSON format)
  \param sysdecl_filename Path to the system declaration file

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  import ctypes
  lib = ctypes.CDLL("libtchecker.so")
  lib.tck_syntax_to_json.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
  output_filename = b"system.json"
  sysdecl_filename = b"system.tck"
  lib.tck_syntax_to_json(output_filename, sysdecl_filename)
  \endcode
*/
void tck_syntax_to_json(const char * output_filename, const char * sysdecl_filename);

/*!
  \brief Creates a synchronized product from a TChecker system declaration

  This function is intended to be called from FFI libraries (e.g., Python via ctypes).
  It creates a synchronized product from the system described in \p sysdecl_filename, names the new system \p new_system_name,
  and writes the result to \p output_filename.

  \param output_filename Path to the output file (synchronized product)
  \param sysdecl_filename Path to the system declaration file
  \param new_system_name Name of the resulting synchronized product system

  \note This function is C-compatible and can be called from Python using ctypes.

  \code{.py}
  import ctypes
  lib = ctypes.CDLL("libtchecker.so")
  lib.tck_syntax_create_synchronized_product.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
  output_filename = b"product.tck"
  sysdecl_filename = b"system.tck"
  new_system_name = b"product_system"
  lib.tck_syntax_create_synchronized_product(output_filename, sysdecl_filename, new_system_name)
  \endcode
*/
void tck_syntax_create_synchronized_product(const char * output_filename, const char * sysdecl_filename, const char * new_system_name);

#ifdef __cplusplus
}
#endif

namespace tchecker {

namespace publicapi {

  /*!
   \brief Check timed automaton syntax from a declaration
   \param output_filename : name of the filename to write the result into. "" for std::cout
   \param sysdecl_filename : the filename of the system declaration
   \post error and warnings have been reported to std::cerr
   */
  void tck_syntax_check_syntax(std::string output_filename, std::string sysdecl_filename);

  /*!
   \brief Output a system of processes following the dot graphviz format
   \param output_filename : name of the filename to write the result into. "" for std::cout
   \param sysdecl_filename : the filename of the system declaration
   \param delimiter : delimiter used in node names
   \post The system of processes in sysdecl has been output to os following the
   dot graphviz format, using delimiter as a separator between process name and
   location name for node names.
  */
  void tck_syntax_to_dot(std::string output_filename, std::string sysdecl_filename, std::string const & delimiter);

  /*!
   \brief Output a system of processes following the JSON format
   \param output_filename : name of the filename to write the result into. "" for std::cout
   \param sysdecl_filename : the filename of the system declaration
   \param delimiter : delimiter used in node names
   \post The system of processes in sysdecl has been output to os following the
   JSON format, using delimiter as a separator between process name and
   location name for node names.
  */
  void tck_syntax_to_json(std::string output_filename, std::string sysdecl_filename, std::string const & delimiter);

  /*!
   \brief Flatten a system of processes into a single process
   \param output_filename : name of the filename to write the result into. "" for std::cout
   \param sysdecl_filename : the filename of the system declaration
   \param new_name) : name of the result
   \param delimiter : delimiter used in names of the resulting process
   \post The synchronized product of the system in sysdecl has been output to output_filename.
   The resulting process has name process_name. The names of locations and events
   are list of location/event names in the original process, seperated by
   delimiter
 */
  void tck_syntax_create_synchronized_product(std::string output_filename, std::string sysdecl_filename, std::string new_name, std::string const & delimiter);

  /*!
   \brief Report asynchronous events from a declaration
   \param sysdecl_filename : the filename of the system declaration
   \post all asynchronous events in sysdecl have been reported to std::cout
  */
  void do_report_asynchronous_events(std::string sysdecl_filename);

} // end of namespace publicapi

} // end of namespace tchecker

#endif
