/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PUBLICAPI_SYNTAX_API_HH
#define TCHECKER_PUBLICAPI_SYNTAX_API_HH

#include <string>

#ifdef __cplusplus
extern "C" {    
#endif

/*! 
 * TODO: doc
 */
void tck_syntax_check_syntax(const char * output_filename, const char * sysdecl_filename);
void tck_syntax_to_dot(const char * output_filename, const char * sysdecl_filename);
void tck_syntax_to_json(const char * output_filename, const char * sysdecl_filename);
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

}
}

#endif
