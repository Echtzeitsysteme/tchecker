#include <fstream>
#include <iostream>

#include "tchecker/parsing/parsing.hh"
#include "tchecker/strong-timed-bisim/stats.hh"
#include "tchecker/strong-timed-bisim/vcg-timed-bisim.hh"
#include "tchecker/system/system.hh"

#include "tchecker/publicapi/tck_compare.hh"

int tck_compare_default_block_size = 10000;
int tck_compare_default_table_size = 65536;

const void tck_compare(const char * output_filename, const char * first_sysdecl_filename, const char * second_sysdecl_filename,
                       tck_compare_relationship_t relationship, int * block_size, int * table_size)
{
  if (block_size == nullptr) {
    block_size = &tck_compare_default_block_size;
  }

  if (table_size == nullptr) {
    table_size = &tck_compare_default_table_size;
  }

  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> first_sysdecl{nullptr};
    first_sysdecl = tchecker::parsing::parse_system_declaration(first_sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> first_system(new tchecker::system::system_t(*first_sysdecl));

    std::shared_ptr<tchecker::parsing::system_declaration_t> second_sysdecl{nullptr};
    second_sysdecl = tchecker::parsing::parse_system_declaration(second_sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> second_system(new tchecker::system::system_t(*second_sysdecl));

    // create output stream to output file

    std::ostream * os = nullptr;
    if (output_filename != "") {
      os = new std::ofstream(output_filename, std::ios::out);
    }
    else
      os = &std::cout;

    if (relationship == STRONG_TIMED_BISIM) {
      tchecker::strong_timed_bisim::stats_t stats =
          tchecker::strong_timed_bisim::run(first_sysdecl, second_sysdecl, os, *block_size, *table_size);

      std::map<std::string, std::string> m;
      stats.attributes(m);
      for (auto && [key, value] : m)
        std::cout << key << " " << value << std::endl;
    }
    else {
      throw std::runtime_error("Unknown relationship");
    }
  }
  catch (std::exception const & e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "Unknown error" << std::endl;
  }
}
