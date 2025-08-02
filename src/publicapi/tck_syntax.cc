#include <fstream>
#include <iostream>

#include "tchecker/parsing/parsing.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syntax-check/syntax-check.hh"
#include "tchecker/system/output.hh"
#include "tchecker/system/system.hh"
#include "tchecker/ta/system.hh"

#include "tchecker/publicapi/tck_syntax.hh"

std::ostream & create_output_stream(const char * output_filename)
{
  std::ostream * os = nullptr;
  if (strcmp(output_filename, "")) { // strcmp returns 0 (which is interpreted as false) if output_filename is "". 
    os = new std::ofstream(output_filename, std::ios::out);
  }
  else
    os = &std::cout;

  return *os;
}

const void tck_syntax_check_syntax(const char * output_filename, const char * sysdecl_filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);

    std::ostream & os = create_output_stream(output_filename);

    tchecker::syntax_check::syntax_check_ta(os, *sysdecl);
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

const void tck_syntax_to_dot(const char * output_filename, const char * sysdecl_filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    std::ostream & os = create_output_stream(output_filename);

    tchecker::system::output_dot(os, *system, "_", tchecker::system::GRAPHVIZ_FULL);
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

const void tck_syntax_to_json(const char * output_filename, const char * sysdecl_filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    std::ostream & os = create_output_stream(output_filename);

    tchecker::system::output_json(os, *system, "_");
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}

const void tck_syntax_create_synchronized_product(const char * output_filename, const char * sysdecl_filename, const char * new_system_name)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::syncprod::system_t> system(new tchecker::syncprod::system_t(*sysdecl));

    std::ostream & os = create_output_stream(output_filename);

    tchecker::system::system_t product = tchecker::syncprod::synchronized_product(system, new_system_name, "_");
    tchecker::system::output_tck(os, product);
  }
  catch (std::exception const & e) {
    std::cerr << e.what() << std::endl;
  }
}
