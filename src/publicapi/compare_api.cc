/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/publicapi/compare_api.hh"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <random>


#include "tchecker/strong-timed-bisim/stats.hh"
#include "tchecker/strong-timed-bisim/vcg-timed-bisim.hh"
#include "tchecker/strong-timed-bisim/certificate/witness/witness_graph.hh"

#include "tchecker/parsing/parsing.hh"
#include "tchecker/system/system.hh"
#include "tchecker/system/static_analysis.hh"

#include "tchecker/publicapi/json_parser.hh"
#include "tchecker/publicapi/reach_api.hh"

#include "tchecker/compare-tools/synchronize.hh"

#include "tchecker/strong-timed-bisim/strategy.hh"

#include "tchecker/ta/system.hh"

#include "tchecker/algorithms/search_order.hh"

void tck_compare(const char * output_filename, 
  const char * first_sysdecl_filename, 
  const char * second_sysdecl_filename,
  tck_compare_relationship_t relationship,
  int * block_size, 
  int * table_size,
  const char * starting_state_attributes_first,
  const char * starting_state_attributes_second,
  const char * inter_constraint,
  bool generate_witness,
  bool all_reachable_states)
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
  std::string inter_constraint_str = (nullptr == inter_constraint) ? std::string("") : std::string(inter_constraint);

  tchecker::publicapi::tck_compare(std::string(output_filename), std::string(first_sysdecl_filename),
                                   std::string(second_sysdecl_filename), relationship, block, table, 
                                   first_state, second_state, inter_constraint_str, generate_witness, all_reachable_states);
}

namespace tchecker {

namespace publicapi {

void strong_timed_bisim(std::ostream & os, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first,
                        std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second, std::size_t block_size,
                        std::size_t table_size, std::map<std::string, std::string> & first_starting_state, 
                        std::map<std::string, std::string> & second_starting_state, 
                        std::string & inter_constraint, bool generate_witness, 
                        std::vector<std::shared_ptr<tchecker::strong_timed_bisim::strategy::state_to_check_t>> & symbolic_states_to_check)
{

  auto stats = tchecker::strong_timed_bisim::run(sysdecl_first, sysdecl_second, &os, block_size, table_size, 
                                                 first_starting_state, second_starting_state, inter_constraint, 
                                                 generate_witness, symbolic_states_to_check);


  if(!symbolic_states_to_check.empty()) {
    std::string name = sysdecl_first->name() + "_" + sysdecl_second->name();
    stats.strategy()->strategy_output(os);
  }

  if(generate_witness) {
    std::string name = sysdecl_first->name() + "_" + sysdecl_second->name();
    if(stats.relationship_fulfilled()) {
      stats.witness()->dot_output(os, name);
    } else {
      stats.counterexample()->dot_output(os, name);
    }
  }
  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;
}

std::string create_temp_filename()
{
  auto tempDir = std::filesystem::temp_directory_path();

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, 999999);

  std::filesystem::path tempFile;
  do {
    tempFile = tempDir / ("tck_reach_certificate_" + std::to_string(dist(gen)));
  } while (std::filesystem::exists(tempFile));

  return tempFile.string();
}

void tck_compare(std::string output_filename, std::string first_sysdecl_filename, std::string second_sysdecl_filename,
                 tck_compare_relationship_t relationship, std::size_t block_size, std::size_t table_size,
                 std::string & first_starting_state_json, std::string & second_starting_state_json, 
                 std::string & inter_constraint, bool generate_witness, bool all_reachable_states)
{
  try {

#if !USE_BOOST_JSON
    if (!first_starting_state_json.empty() || !second_starting_state_json.empty()) {
      std::cerr << "JSON display is not enabled in this build" << std::endl;
      return;
    }
    if (all_reachable_states) {
      std::cerr << "All reachable states can only be checked, if JSON is available, which is not the case in this build." << std::endl;
    }
#endif

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

    std::map<std::string, std::string> first_starting_state_attributes, second_starting_state_attributes;
    std::vector<std::shared_ptr<tchecker::strong_timed_bisim::strategy::state_to_check_t>> reachable_states;


#if USE_BOOST_JSON
    if (!first_starting_state_json.empty()) {
      first_starting_state_attributes = parse_state_json(first_starting_state_json);
    }
    if (!second_starting_state_json.empty()) {
      second_starting_state_attributes = parse_state_json(second_starting_state_json);
    }

    if(all_reachable_states) {

      // Synchronize the systems
      std::string synced_sysdecl_filename = create_temp_filename() + std::string(".tck");
      tchecker::compare_tools::syncer_t(first_sysdecl_filename, second_sysdecl_filename, synced_sysdecl_filename);
      std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl
        = tchecker::parsing::parse_system_declaration(synced_sysdecl_filename);


      std::shared_ptr<tchecker::ta::system_t const> system = std::make_shared<tchecker::ta::system_t const>(*sysdecl);
      if (!tchecker::system::every_process_has_initial_location(system->as_system_system()))
        std::cerr << tchecker::log_warning << "system has no initial state" << std::endl;

      std::shared_ptr<tchecker::zg::zg_t> zg{tchecker::zg::factory(system, tchecker::ts::SHARING, tchecker::zg::ELAPSED_SEMANTICS,
                                                                tchecker::zg::EXTRA_M_GLOBAL,
                                                                block_size, table_size)};

      std::shared_ptr<tchecker::algorithms::zg_reach::state_space_t> state_space =
        std::make_shared<tchecker::algorithms::zg_reach::state_space_t>(zg, block_size, table_size);

      boost::dynamic_bitset<> accepting_labels = system->as_syncprod_system().labels(std::string(""));

      tchecker::algorithms::zg_reach::algorithm_t algorithm;

      enum tchecker::waiting::policy_t policy = tchecker::algorithms::waiting_policy(std::string("bfs"));

      algorithm.run(state_space->zg(), state_space->graph(), accepting_labels, policy);

      //tchecker::algorithms::zg_reach::dot_output(*os, state_space->graph(), sysdecl->name());
      // remove the file
      std::filesystem::remove(synced_sysdecl_filename);

      // convert the reachable nodes to a set of nodes.
      auto nodes =  state_space->graph().nodes();

      // We need to hard copy the states, as the state_space also holds the zone graph, which holds the pool allocator
      // and everything explodes if we still hold shared pointers to objects over there.
      for(auto cur : nodes) {
        reachable_states.emplace_back(
          std::make_shared<tchecker::strong_timed_bisim::strategy::state_to_check_t>(
              tchecker::to_string(cur->state_ptr()->vloc(), state_space->zg().system()), 
              cur->state_ptr()->intval(), 
              cur->state_ptr()->zone_ptr()));
      }

    }
#endif

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

    if (relationship == STRONG_TIMED_BISIM) {
      strong_timed_bisim(*os, first_sysdecl, second_sysdecl, block_size, table_size, 
                         first_starting_state_attributes, second_starting_state_attributes,
                         inter_constraint, generate_witness, reachable_states);
    }
    else {
      std::cerr << tchecker::log_error << "Unknown relationship" << std::endl;
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
