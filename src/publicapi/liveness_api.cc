/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/publicapi/liveness_api.hh"

#include <fstream>
#include <iostream>

#include "tchecker/algorithms/couvreur_scc/zg-couvscc.hh"
#include "tchecker/algorithms/ndfs/zg-ndfs.hh"

#include "tchecker/parsing/parsing.hh"
#include "tchecker/system/system.hh"

void tck_liveness(const char * output_filename, const char * sysdecl_filename, const char * labels,
                  tck_liveness_algorithm_t algorithm, tck_liveness_certificate_t certificate, int * block_size,
                  int * table_size)
{
  std::size_t block = TCK_LIVENESS_INIT_BLOCK_SIZE;
  if (nullptr != block_size) {
    block = *block_size;
  }

  std::size_t table = TCK_LIVENESS_INIT_TABLE_SIZE;
  if (nullptr != table_size) {
    table = *table_size;
  }

  tchecker::publicapi::tck_liveness(std::string(output_filename), std::string(sysdecl_filename), std::string(labels), algorithm,
                                    certificate, block, table);
}

namespace tchecker {
namespace publicapi {

/*!
 \brief Check if expected certificate is a path
 \param ctype : certificate type
 \return true if ctype is a path, false otherwise
 */
static bool is_certificate_path(enum tck_liveness_certificate_t ctype) { return (ctype == CERTIFICATE_SYMBOLIC); }

/*!
 \brief Run nested DFS algorithm
 \param sysdecl : system declaration
 \post statistics on accepting run w.r.t. command-line specified labels in
 the system declared by sysdecl have been output to standard output.
 A certificate has been output if required.
*/
const void tck_liveness_zg_ndfs(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl, std::string labels,
                                std::size_t block_size, std::size_t table_size, tck_liveness_certificate_t certificate)
{
  auto && [stats, state_space] = tchecker::algorithms::zg_ndfs::run(sysdecl, labels, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::algorithms::zg_ndfs::dot_output(os, state_space->graph(), sysdecl.name());
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.cycle()) {
    std::unique_ptr<tchecker::algorithms::zg_ndfs::cex::symbolic_cex_t> cex{
        tchecker::algorithms::zg_ndfs::cex::symbolic_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("*** tck_liveness: unable to compute a symbolic counter example for ndfs algorithm");
    tchecker::algorithms::zg_ndfs::cex::dot_output(os, *cex, sysdecl.name());
  }
}

/*!
 \brief Run Couvreur's algorithm
 \param sysdecl : system declaration
 \post statistics on accepting run w.r.t. command-line specified labels in
 the system declared by sysdecl have been output to standard output.
 A certificate has been output if required.
*/
const void tck_liveness_zg_couvscc(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                                   std::string labels, int block_size, int table_size, tck_liveness_certificate_t certificate)
{
  std::string labels_str(labels);
  std::string::difference_type labels_count = std::count(labels_str.begin(), labels_str.end(), ',') + 1;

  if (is_certificate_path(certificate) && labels_count > 1)
    throw std::runtime_error(
        "*** tck_liveness: cannot compute symbolic counter example with more than 1 label (use graph instead)");

  auto && [stats, state_space] = tchecker::algorithms::zg_couvscc::run(sysdecl, labels, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::algorithms::zg_couvscc::dot_output(os, state_space->graph(), sysdecl.name());
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.cycle()) {
    std::unique_ptr<tchecker::algorithms::zg_couvscc::cex::symbolic_cex_t> cex{
        tchecker::algorithms::zg_couvscc::cex::symbolic_counter_example(state_space->graph())};
    if (cex->empty())
      throw std::runtime_error("*** tck_liveness: unable to compute a symbolic counter example for couvscc algorithm");
    tchecker::algorithms::zg_couvscc::cex::dot_output(os, *cex, sysdecl.name());
  }
}

void tck_liveness(std::string output_filename, std::string sysdecl_filename, std::string labels,
                        tck_liveness_algorithm_t algorithm, tck_liveness_certificate_t certificate, std::size_t block_size,
                        std::size_t table_size)
{
  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    if (sysdecl == nullptr) {
      throw std::runtime_error("nullptr system declaration");
    }
    std::shared_ptr<tchecker::system::system_t> system = std::make_shared<tchecker::system::system_t>(*sysdecl);

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

    if (algorithm == ALGO_COUVSCC) {
      tck_liveness_zg_couvscc(*os, *sysdecl, labels, block_size, table_size, certificate);
    }
    else if (algorithm == ALGO_NDFS) {
      tck_liveness_zg_ndfs(*os, *sysdecl, labels, block_size, table_size, certificate);
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