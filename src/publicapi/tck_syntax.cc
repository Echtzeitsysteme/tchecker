/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/publicapi/tck_syntax.hh"

#include <fstream>
#include <iostream>

#include "tchecker/parsing/parsing.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syntax-check/syntax-check.hh"
#include "tchecker/system/output.hh"
#include "tchecker/system/system.hh"
#include "tchecker/ta/system.hh"

/*!
 \brief Type of pair (process identifier, event identifier)
*/
using process_event_t = std::tuple<tchecker::process_id_t, tchecker::event_id_t>;

/*!
 \brief Hash functor on process events
*/
class hash_process_event_t {
public:
  /*!
   \brief Hash function
   \param process_event : process event
   \return hash code for process_event
  */
  std::size_t operator()(process_event_t const & process_event) const
  {
    std::size_t h = boost::hash_value(std::get<0>(process_event));
    boost::hash_combine(h, std::get<1>(process_event));
    return h;
  }
};

void tck_syntax_check_syntax(const char * output_filename, const char * sysdecl_filename)
{
  tchecker::publicapi::tck_syntax_check_syntax(std::string(output_filename), std::string(sysdecl_filename));
}

void tck_syntax_to_dot(const char * output_filename, const char * sysdecl_filename)
{
  tchecker::publicapi::tck_syntax_to_dot(std::string(output_filename), std::string(sysdecl_filename), "_");
}

void tck_syntax_to_json(const char * output_filename, const char * sysdecl_filename)
{
  tchecker::publicapi::tck_syntax_to_json(std::string(output_filename), std::string(sysdecl_filename), "_");
}
void tck_syntax_create_synchronized_product(const char * output_filename, const char * sysdecl_filename, const char * new_system_name)
{
  tchecker::publicapi::tck_syntax_create_synchronized_product(std::string(output_filename), std::string(sysdecl_filename), 
                                                              std::string(new_system_name), "_");
}

namespace tchecker {

namespace publicapi {

std::ostream & create_output_stream(std::string output_filename)
{
  std::ostream * os = nullptr;
  if (output_filename != "") { // strcmp returns 0 (which is interpreted as false) if output_filename is "". 
    os = new std::ofstream(output_filename, std::ios::out);
  }
  else
    os = &std::cout;

  return *os;
}

void tck_syntax_check_syntax(std::string output_filename, std::string sysdecl_filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);

    std::ostream & os = create_output_stream(output_filename);

    if(tchecker::syntax_check::syntax_check_ta(os, *sysdecl)) {
      os << "Syntax OK" << std::endl;
    }
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

void tck_syntax_to_dot(std::string output_filename, std::string sysdecl_filename, std::string const & delimiter)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    std::ostream & os = create_output_stream(output_filename);

    tchecker::system::output_dot(os, *system, delimiter, tchecker::system::GRAPHVIZ_FULL);
    os << std::endl;
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

void tck_syntax_to_json(std::string output_filename, std::string sysdecl_filename, std::string const & delimiter)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    std::ostream & os = create_output_stream(output_filename);

    tchecker::system::output_json(os, *system, delimiter);
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

void tck_syntax_create_synchronized_product(std::string output_filename, std::string sysdecl_filename, std::string new_name, std::string const & delimiter)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::syncprod::system_t> system(new tchecker::syncprod::system_t(*sysdecl));

    std::ostream & os = create_output_stream(output_filename);

    tchecker::system::system_t product = tchecker::syncprod::synchronized_product(system, new_name, delimiter);
    tchecker::system::output_tck(os, product);
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

void do_report_asynchronous_events(std::string sysdecl_filename)
{
  std::unordered_set<process_event_t, hash_process_event_t> reported_asynchronous_events;

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    tchecker::syncprod::system_t system(*sysdecl);

    std::cout << "Asynchronous events in model " << system.name() << std::endl;
    for (tchecker::system::edge_const_shared_ptr_t const & edge : system.edges())
      if (system.is_asynchronous(*edge)) {
        process_event_t process_event = std::make_tuple(edge->pid(), edge->event_id());
        if (reported_asynchronous_events.find(process_event) == reported_asynchronous_events.end()) {
          std::cout << "    event " << system.event_name(edge->event_id()) << " in process " << system.process_name(edge->pid())
                    << std::endl;
          reported_asynchronous_events.insert(process_event);
        }
      }
    std::cout << "Found " << reported_asynchronous_events.size() << " asynchronous event(s)" << std::endl;
  }
  catch (...) {
    std::cerr << tchecker::log_error << "Syntax error in TChecker file (run tck-syntax with option -c)" << std::endl;
  }
}


} // end of namespace publicapi

} // end of namespace tchecker