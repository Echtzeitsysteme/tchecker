/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_COMPARE_TOOLS_SYNCHRONIZE_HH
#define TCHECKER_COMPARE_TOOLS_SYNCHRONIZE_HH

#include <string>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/ta/state.hh"
#include "tchecker/zg/zone_container.hh"

namespace tchecker {

namespace compare_tools {

/*!
 \class syncer_t
 \brief Can be used to create one system from two.
 */
class syncer_t {

 public:
  /*!
   \brief takes two system declarations and writes a version, in which the same events are synchronized, into the file_to_write
   \param first_sysdecl_filename : the first system declaration
   \param second_sysdecl_filename : the second system declaration
   \param first_filename_replace : the target, where the synchronous product of first_sysdecl_filename should be stored
   \param second_filename_replace : the target, where the synchronous product of second_sysdecl_filename should be stored
   \param filte_to_write : the file to write the target into
  */
  syncer_t(const std::string & first_sysdecl_filename, 
           const std::string & second_sysdecl_filename,
           const std::string & first_filename_replace,
           const std::string & second_filename_replace,
           const std::string & file_to_write);

 
 private:
  /*!
   \brief Calls the product functionality for the given model
   \param filename_replace : where the product shall be stored
   \param filename : where the model is stored
   \return the path to the product
   \note This method is used by the constructor. Do not change it s.t. any state variable is used
   */
  std::string flatten_system(const std::string filename_replace, const std::string & filename);
  /*!
   \brief Reads the file content of a file and returns it
   \param filename : The file to read
   \note This method is used by the constructor. Do not change it s.t. any state variable is used!
   */
  std::string read_file_content(const std::string & filename);

  /*!
   \brief Writes the synced system into the _file_to_write
   \post The synced system is written into the file_to_write
   */
  void write_file_content();

  /*!
   \brief Gets a content string and returns all occurrences of a specific reges.
   \param content : The content in which the regex shall be searched
   \param regex : the regex to search for
   \param to_take : which part of the regex should be returned
   \note This method is used by the constructor. Do not change it s.t. any state variable is used!
  */
  std::vector<std::string> find_occurrences_regex(std::string & content, const std::string & regex, std::size_t to_take=2);

  /*!
   \brief Replaces the occurrences of a vector of string by the replacement
   \param content : A pointer to the string that should be changed
   \param to_replace : The expressions that should be replaced
   \param regex_before : The regex that should be placed before the expressions when searched for them
   \param regex_after : The regex that should be placed after the expressions when searched for them
   \param replacement_before : The string that should be added before the expression
   \param replacement_after : The string that should be added after the expression
   \post content is changed
   \note This method is used by the constructor. Do not change it s.t. any state variable is used!
   */
  void replace_occurrences(std::string * content, const std::vector<std::string> & to_replace, 
                           const std::string & regex_before, const std::string & regex_after,
                           const std::string & replacement_before, const std::string & replacement_after);
  std::pair<std::string, std::string> _content;

  std::pair<std::vector<std::string>, std::vector<std::string>> _processes;
  std::pair<std::vector<std::string>, std::vector<std::string>> _events;
  std::pair<std::vector<std::string>, std::vector<std::string>> _clocks;
  std::pair<std::vector<std::string>, std::vector<std::string>> _ints;

  const std::string _file_to_write;

  std::string _syncs;
};

} // end of namespace compare_tools

} // end of namespace tchecker
#endif