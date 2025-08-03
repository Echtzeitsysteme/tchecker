/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/publicapi/tck_reach.hh"

#include <fstream>
#include <iostream>

#include "tchecker/algorithms/concur19/concur19.hh"
#include "tchecker/algorithms/covreach/zg-covreach.hh"
#include "tchecker/algorithms/reach/zg-reach.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/system/system.hh"

#include "../src/tck-reach/zg-aLU-covreach.hh" // TODO: to be moved to algorithms.

void tck_reach(const char * output_filename, 
               const char * sysdecl_filename, 
               const char * labels, 
               tck_reach_algorithm_t algorithm, 
               const char * search_order, 
               tck_reach_certificate_t certificate, 
               int* block_size, 
               int* table_size)
{
  std::size_t block = TCK_REACH_INIT_BLOCK_SIZE;
  if(nullptr != block_size) {
    block = *block_size;
  }

  std::size_t table = TCK_REACH_INIT_TABLE_SIZE;
  if(nullptr != table_size) {
    table = *table_size;
  }


  tchecker::publicapi::tck_reach(std::string(output_filename), std::string(sysdecl_filename), std::string(labels), algorithm, std::string(search_order), certificate, block, table);              
}

namespace tchecker {

namespace publicapi {

/*!
 \brief Check if expected certificate is a path
 \param ctype : certificate type
 \return true if ctype is a path, false otherwise
 */
static bool is_certificate_path(enum tck_reach_certificate_t ctype)
{
  return (ctype == CERTIFICATE_SYMBOLIC || ctype == CERTIFICATE_CONCRETE);
}

/*!
 \brief Perform reachability analysis
 \param sysdecl : system declaration
 \post statistics on reachability analysis of command-line specified labels in
 the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
*/
void tck_reach_zg_reach(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl, std::string labels,
                              const char * search_order, int block_size, int table_size, tck_reach_certificate_t certificate)
{
  auto && [stats, state_space] = tchecker::algorithms::zg_reach::run(sysdecl, labels, search_order, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::algorithms::zg_reach::dot_output(os, state_space->graph(), sysdecl.name());
  else if ((certificate == CERTIFICATE_CONCRETE) && stats.reachable()) {
    std::unique_ptr<tchecker::algorithms::zg_reach::cex::concrete_cex_t> cex{
        tchecker::algorithms::zg_reach::cex::concrete_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a concrete counter example");
    tchecker::algorithms::zg_reach::cex::dot_output(os, *cex, sysdecl.name());
  }
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::algorithms::zg_reach::cex::symbolic_cex_t> cex{
        tchecker::algorithms::zg_reach::cex::symbolic_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::algorithms::zg_reach::cex::dot_output(os, *cex, sysdecl.name());
  }
}

/*!
 \brief Perform covering reachability analysis over the local-time zone graph
 \param sysdecl : system declaration
 \post statistics on covering reachability analysis of command-line specified
 labels in the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
 \note This is the algorithm presented in R. Govind, Frédéric Herbreteau, B.
 Srivathsan, Igor Walukiewicz: "Revisiting Local Time Semantics for Networks of
 Timed Automata". CONCUR 2019: 16:1-16:15
*/
void tck_reach_concur19(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl, std::string labels,
                              std::string search_order, int block_size, int table_size, tck_reach_certificate_t certificate)
{
  if (certificate == CERTIFICATE_CONCRETE)
    throw std::runtime_error("Concrete counter-example is not available for concur19 algorithm");

  tchecker::algorithms::covreach::covering_t covering =
      (is_certificate_path(certificate) ? tchecker::algorithms::covreach::COVERING_LEAF_NODES
                                        : tchecker::algorithms::covreach::COVERING_FULL);

  auto && [stats, state_space] =
      tchecker::algorithms::concur19::run(sysdecl, labels, search_order, covering, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::algorithms::concur19::dot_output(os, state_space->graph(), sysdecl.name());
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::algorithms::concur19::cex::symbolic::cex_t> cex{
        tchecker::algorithms::concur19::cex::symbolic::counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::algorithms::concur19::cex::symbolic::dot_output(os, *cex, sysdecl.name());
  }
}

/*!
 \brief Perform covering reachability analysis
 \param sysdecl : system declaration
 \post statistics on covering reachability analysis of command-line specified
 labels in the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
*/
void tck_reach_zg_covreach(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                           std::string labels, const char * search_order, int block_size, int table_size,
                           tck_reach_certificate_t certificate)
{
  tchecker::algorithms::covreach::covering_t covering =
      (is_certificate_path(certificate) ? tchecker::algorithms::covreach::COVERING_LEAF_NODES
                                        : tchecker::algorithms::covreach::COVERING_FULL);
  auto && [stats, state_space] =
      tchecker::algorithms::zg_covreach::run(sysdecl, labels, search_order, covering, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::algorithms::zg_covreach::dot_output(os, state_space->graph(), sysdecl.name());
  else if ((certificate == CERTIFICATE_CONCRETE) && stats.reachable()) {
    std::unique_ptr<tchecker::algorithms::zg_covreach::cex::concrete_cex_t> cex{
        tchecker::algorithms::zg_covreach::cex::concrete_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a concrete counter example");
    tchecker::algorithms::zg_covreach::cex::dot_output(os, *cex, sysdecl.name());
  }
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::algorithms::zg_covreach::cex::symbolic_cex_t> cex{
        tchecker::algorithms::zg_covreach::cex::symbolic_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::algorithms::zg_covreach::cex::dot_output(os, *cex, sysdecl.name());
  }
}

/*!
 \brief Perform covering reachability analysis with aLU subsumption
 \param sysdecl : system declaration
 \post statistics on aLU covering reachability analysis of command-line specified
 labels in the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
*/
void tck_reach_zg_alu_covreach(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                               std::string labels, const char * search_order, int block_size, int table_size,
                               tck_reach_certificate_t certificate)
{
  tchecker::algorithms::covreach::covering_t covering =
      (is_certificate_path(certificate) ? tchecker::algorithms::covreach::COVERING_LEAF_NODES
                                        : tchecker::algorithms::covreach::COVERING_FULL);
           
  std::shared_ptr<tchecker::parsing::system_declaration_t> const sysdecl_ptr = std::make_shared<tchecker::parsing::system_declaration_t>(sysdecl);
  auto && [stats, state_space] =
      tchecker::tck_reach::zg_alu_covreach::run(sysdecl_ptr, labels, search_order, covering, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::tck_reach::zg_alu_covreach::dot_output(os, state_space->graph(), sysdecl_ptr->name());
  else if ((certificate == CERTIFICATE_CONCRETE) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::zg_alu_covreach::cex::concrete_cex_t> cex{
        tchecker::tck_reach::zg_alu_covreach::cex::concrete_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a concrete counter example");
    tchecker::tck_reach::zg_alu_covreach::cex::dot_output(os, *cex, sysdecl_ptr->name());
  }
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::zg_alu_covreach::cex::symbolic_cex_t> cex{
        tchecker::tck_reach::zg_alu_covreach::cex::symbolic_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::tck_reach::zg_alu_covreach::cex::dot_output(os, *cex, sysdecl_ptr->name());
  }
}

void tck_reach(std::string output_filename, std::string sysdecl_filename, std::string labels, tck_reach_algorithm_t algorithm,
               std::string search_order, tck_reach_certificate_t certificate, std::size_t  block_size, std::size_t table_size)
{

  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    if (sysdecl == nullptr) {
      throw std::runtime_error("nullptr system declaration");
    }
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    // create output stream to output file

    std::ostream * os = nullptr;
    if (output_filename != "") { 
      os = new std::ofstream(output_filename, std::ios::out);
    }
    else {
      os = &std::cout;
    }

    if (search_order != "bfs" && search_order != "dfs") {
      throw std::runtime_error("Unknown search order");
    }

    if (algorithm == ALGO_REACH) {
      tck_reach_zg_reach(*os, *sysdecl, labels, search_order.c_str(), block_size, table_size, certificate);
    }
    else if (algorithm == ALGO_CONCUR19) {
      tck_reach_concur19(*os, *sysdecl, labels, search_order.c_str(), block_size, table_size, certificate);
    }
    else if (algorithm == ALGO_COVREACH) {
      tck_reach_zg_covreach(*os, *sysdecl, labels, search_order.c_str(), block_size, table_size, certificate);
    }
    else if (algorithm == ALGO_ALU_COVREACH) {
      tck_reach_zg_alu_covreach(*os, *sysdecl, labels, search_order.c_str(), block_size, table_size, certificate);
    }
    else {
      throw std::runtime_error("Unknown algorithm");
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