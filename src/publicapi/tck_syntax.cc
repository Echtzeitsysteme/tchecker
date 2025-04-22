#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>

#include "tchecker/parsing/parsing.hh"
#include "tchecker/publicapi/tck_syntax.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/system/output.hh"
#include "tchecker/system/system.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"
// #include "tchecker/publicapi/syntax-check.hh"

const char * tck_syntax_check_syntax(const char * filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
    // std::ostringstream oss;
    // tchecker::tck_syntax::syntax_check_ta(oss, *sysdecl);
    
    // std::string result = oss.str();

    // char * c_output = (char *)std::malloc(result.size() + 1);
    // if (c_output == nullptr)
    //   return nullptr; // malloc failed

    // std::strcpy(c_output, result.c_str());
    // return c_output;
    return "Success";
    
  }
  catch (std::exception const & e) {
    return e.what();
  }
}

const char * tck_syntax_to_dot(const char * filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(* sysdecl));

    std::ostringstream oss;

    tchecker::system::output_dot(oss, *system, "_");

    std::string result = oss.str();

    char * c_output = (char *)std::malloc(result.size() + 1);
    if (c_output == nullptr)
      return nullptr; // malloc failed

    std::strcpy(c_output, result.c_str());
    return c_output;
  }
  catch (std::exception const & e) {
    return e.what();
  }
}

const char * tck_syntax_to_json(const char * filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(* sysdecl));

    std::ostringstream oss;

    tchecker::system::output_json(oss, *system, "_");

    std::string result = oss.str();

    char * c_output = (char *)std::malloc(result.size() + 1);
    if (c_output == nullptr)
      return nullptr; // malloc failed

    std::strcpy(c_output, result.c_str());
    return c_output;
  }
  catch (std::exception const & e) {
    return e.what();
  }
}

const char * tck_syntax_create_synchronized_product(const char * filename, const char * process_name)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
    std::shared_ptr<tchecker::syncprod::system_t> system(new tchecker::syncprod::system_t(*sysdecl));

    std::ostringstream oss;

    tchecker::system::system_t product = tchecker::syncprod::synchronized_product(system, process_name, "_");
    tchecker::system::output_tck(oss, product);

    std::string result = oss.str();

    char * c_output = (char *)std::malloc(result.size() + 1);
    if (c_output == nullptr)
      return nullptr; // malloc failed

    std::strcpy(c_output, result.c_str());
    return c_output;
  }
  catch (std::exception const & e) {
    return e.what();
  }

}