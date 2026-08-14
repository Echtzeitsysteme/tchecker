/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/compare-tools/synchronize.hh"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <stdexcept>

#include "tchecker/publicapi/syntax_api.hh"
#include "tchecker/utils/tmp_file.hh"

// \b means that this is the beginning of the word. Therefore, we do not consider "thisprocess", or any analog stupid name.
// Then we have a second term, which is the name of the process and wich can consist of capitals, small letters, numbers, and underscore. 
// However, the first letter is not allowed to be a number.
#define PROCESS_REGEX std::string(R"((\bprocess:)([A-Za-z_][A-Za-z0-9_]*))")

#define EVENT_REGEX std::string(R"((\bevent:)([A-Za-z_][A-Za-z0-9_]*))")

// colon is delimiter. Afterwards, the size of the clock is given, which can be any number. Once again: Colon is delimiter.
// Then we have a third term, which is the name of the clock and wich can consist of capitals, small letters, numbers, and underscore. 
// However, the first letter is not allowed to be a number. 
#define CLOCK_REGEX std::string(R"((\bclock:)([0-9]+:)([A-Za-z_][A-Za-z0-9_]*))")

// colon is delimiter. Afterwards, the size of the int is given, followed by min, max, and init values.
// Then we have a sixth term, which is the name of the clock and wich can consist of capitals, small letters, numbers, and underscore. 
// However, the first letter is not allowed to be a number.
#define INT_REGEX std::string(R"((\bint:)([0-9]+:)(-?[0-9]+:)(-?[0-9]+:)(-?[0-9]+:)([A-Za-z_][A-Za-z0-9_]*))")

namespace tchecker {

namespace compare_tools {

syncer_t::syncer_t(const std::string & first_sysdecl_filename,
                   const std::string & second_sysdecl_filename, 
                   const std::string & first_filename_replace,
                   const std::string & second_filename_replace,
                   const std::string & file_to_write)
  : _content(std::make_pair(read_file_content(flatten_system(first_filename_replace, first_sysdecl_filename)), read_file_content(flatten_system(second_filename_replace, second_sysdecl_filename)))),
    _processes(std::make_pair(find_occurrences_regex(_content.first, PROCESS_REGEX), find_occurrences_regex(_content.second, PROCESS_REGEX))),
    _events(std::make_pair(find_occurrences_regex(_content.first, EVENT_REGEX), find_occurrences_regex(_content.second, EVENT_REGEX))),
    _clocks(std::make_pair(find_occurrences_regex(_content.first, CLOCK_REGEX, 3), find_occurrences_regex(_content.second, CLOCK_REGEX, 3))),
    _ints(std::make_pair(find_occurrences_regex(_content.first, INT_REGEX, 6), find_occurrences_regex(_content.second, INT_REGEX, 6))),
    _file_to_write(file_to_write)
{
  std::vector<std::string> postfixes;
  postfixes.emplace_back(std::string("_1"));
  postfixes.emplace_back(std::string("_2"));
  // we need to make all processes, events, clocks, and ints unique. Therefore, we add a postfix _1 or _2 to all of them.
  for(std::size_t i = 0; i < 2; ++i) {
    std::string * cur_content = (0 == i) ? (&_content.first) : (&_content.second);

    // processes
    const std::vector<std::string> * cur_processes = (0 == i) ? (&_processes.first) : (&_processes.second);
    // e.g., location:process:name or edge:process:name
    replace_occurrences(cur_content, *cur_processes, std::string("\\b"),  std::string("\\b"), std::string(), postfixes[i]);
    // e.g., sync:process@event
    replace_occurrences(cur_content, *cur_processes, std::string("\\b"), std::string("@"), std::string(), postfixes[i] + std::string("@"));

    // events
    const std::vector<std::string> * cur_events = (0 == i) ? (&_events.first) : (&_events.second);
    // e.g., edge:process:startloc:targetloc:event{}
    replace_occurrences(cur_content, *cur_events, std::string("\\b"), std::string("\\b"), std::string(), postfixes[i]);
    // e.g., sync:process@event
    replace_occurrences(cur_content, *cur_events, std::string("@"), std::string("\\b"), std::string("@"), postfixes[i]);

    // clocks
    replace_occurrences(cur_content, (0 == i) ? (_clocks.first) : (_clocks.second), std::string("\\b"), std::string("\\b"), std::string(), postfixes[i]);

    // ints
    replace_occurrences(cur_content, (0 == i) ? (_ints.first) : (_ints.second), std::string("\\b"), std::string("\\b"), std::string(), postfixes[i]); 

    // remove the "system:" line
    std::size_t begin = cur_content->find("system:");
    std::size_t end = cur_content->find('\n', begin);
    cur_content->erase(begin, end-begin+1);
  }

  std::sort(_events.first.begin(), _events.first.end());
  std::sort(_events.second.begin(), _events.second.end());

  std::vector<std::string> result;

  std::set_intersection(
    _events.first.begin(), _events.first.end(),
    _events.second.begin(), _events.second.end(),
    std::back_inserter(result)
  );

  for(std::string event : result) {
    for(std::string process_1 : _processes.first) {
      for(std::string process_2 : _processes.second) {
        _syncs += std::string("sync:") 
                  + process_1 + postfixes[0] + std::string("@") + event + postfixes[0] + std::string(":")
                  + process_2 + postfixes[1] + std::string("@") + event + postfixes[1] + "\n";
      }
    }
  }

  write_file_content();
}


std::string syncer_t::flatten_system(const std::string filename_replace, const std::string & filename)
{
  tchecker::publicapi::tck_syntax_create_synchronized_product(filename_replace, filename, "product", "_");
  return filename_replace;
}

std::string syncer_t::read_file_content(const std::string & filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}

void syncer_t::write_file_content() {
  std::ofstream file(_file_to_write);
  if (!file.is_open())
    throw std::runtime_error("Cannot open file: " + _file_to_write); // failed to open for writing

  file << "system:synchronized" << std::endl << std::endl;
  file << _content.first << std::endl;
  file << _content.second << std::endl;
  file << _syncs << std::endl;

  file.close();
}

std::vector<std::string> syncer_t::find_occurrences_regex(std::string & content, const std::string & regex, std::size_t to_take)
{
  std::regex decl(regex);
  std::vector<std::string> result;
  for (auto it = std::sregex_iterator(content.begin(), content.end(), decl); it != std::sregex_iterator(); ++it) {
    result.push_back((*it)[to_take].str());
  }
  return result;
}


void syncer_t::replace_occurrences(std::string * content, const std::vector<std::string> & to_replace, 
                                   const std::string & regex_before, const std::string & regex_after,
                                   const std::string & replacement_before, const std::string & replacement_after)
{
  for (const auto& cur : to_replace) {
    std::regex process(regex_before + cur + regex_after);
    *content = std::regex_replace(*content, process, replacement_before + cur + replacement_after);
  }  
}

} // end of namespace compare_tools

} // end of namespace tchecker