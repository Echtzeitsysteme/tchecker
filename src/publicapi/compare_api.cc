/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/publicapi/compare_api.hh"

#include <fstream>
#include <iostream>

#include "tchecker/strong-timed-bisim/stats.hh"
#include "tchecker/strong-timed-bisim/vcg-timed-bisim.hh"
#include "tchecker/strong-timed-bisim/certificate/witness/witness_graph.hh"

#include "tchecker/parsing/parsing.hh"
#include "tchecker/system/system.hh"

void tck_compare(const char * output_filename, 
  const char * first_sysdecl_filename, 
  const char * second_sysdecl_filename,
  tck_compare_relationship_t relationship,
  int * block_size, 
  int * table_size,
  const char * starting_state_attributes_first,
  const char * starting_state_attributes_second,
  bool generate_witness)
{
  std::size_t block = TCK_COMPARE_INIT_BLOCK_SIZE;
  if (nullptr != block_size) {
    block = *block_size;
  }

  std::size_t table = TCK_COMPARE_INIT_TABLE_SIZE;
  if (nullptr != table_size) {
    table = *table_size;
  }

  std::string first_state = (nullptr == starting_state_attributes_first) ? std::string("") : std::string(starting_state_attributes_first);
  std::string second_state = (nullptr == starting_state_attributes_second) ? std::string("") : std::string(starting_state_attributes_second);

  tchecker::publicapi::tck_compare(std::string(output_filename), std::string(first_sysdecl_filename),
                                   std::string(second_sysdecl_filename), relationship, block, table, 
                                   first_state, second_state, generate_witness);
}

namespace tchecker {

namespace publicapi {

void strong_timed_bisim(std::ostream & os, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first,
                        std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second, std::size_t block_size,
                        std::size_t table_size,  std::string first_starting_state_json, std::string second_starting_state_json,
                        bool generate_witness)
{

  auto stats = tchecker::strong_timed_bisim::run(sysdecl_first, sysdecl_second, &os, block_size, table_size, 
                                                 first_starting_state_json, second_starting_state_json, generate_witness);

  if(generate_witness) {
    if(stats.relationship_fulfilled()) {
      std::string name = sysdecl_first->name() + "_" + sysdecl_second->name();
      stats.witness()->dot_output(os, name);
    } else {
      std::string name = sysdecl_first->name() + "_" + sysdecl_second->name();
      stats.counterexample()->dot_output(os, name);
    }
  }
  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;
}

void tck_compare(std::string output_filename, std::string first_sysdecl_filename, std::string second_sysdecl_filename,
                 tck_compare_relationship_t relationship, std::size_t block_size, std::size_t table_size,
                 std::string first_starting_state_json, std::string second_starting_state_json, bool generate_witness)
{
  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> first_sysdecl{nullptr};
    first_sysdecl = tchecker::parsing::parse_system_declaration(first_sysdecl_filename);
    if (first_sysdecl == nullptr) {
      throw std::runtime_error("nullptr first system declaration");
    }
    std::shared_ptr<tchecker::system::system_t> first_system = std::make_shared<tchecker::system::system_t>(*first_sysdecl);

    std::shared_ptr<tchecker::parsing::system_declaration_t> second_sysdecl{nullptr};
    second_sysdecl = tchecker::parsing::parse_system_declaration(second_sysdecl_filename);
    if (second_sysdecl == nullptr) {
      throw std::runtime_error("nullptr system declaration");
    }
    std::shared_ptr<tchecker::system::system_t> second_system = std::make_shared<tchecker::system::system_t>(*second_sysdecl);

    // create output stream to output file

    std::ostream * os = nullptr;
    std::ofstream ofs;

    if (output_filename != "") {
      ofs.open(output_filename, std::ios::out);
      if (!ofs) {
        throw std::runtime_error("Failed to open file: " + output_filename);
      }
      os = &ofs;
    }
    else {
      os = &std::cout;
    }

    if (relationship == STRONG_TIMED_BISIM) {
      strong_timed_bisim(*os, first_sysdecl, second_sysdecl, block_size, table_size, 
                          first_starting_state_json, second_starting_state_json, generate_witness);
    }
    else {
      throw std::runtime_error("Unknown relationship");
    }
  }
  catch (std::runtime_error & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << tchecker::log_error << "Unknown error" << std::endl;
  }
}

} // end of namespace publicapi

} // end of namespace tchecker
