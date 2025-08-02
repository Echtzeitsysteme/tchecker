#include <fstream>
#include <iostream>

#include "tchecker/algorithms/couvreur_scc/zg-couvscc.hh"
#include "tchecker/algorithms/ndfs/zg-ndfs.hh"
#include "tchecker/algorithms/reach/zg-reach.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/system/system.hh"

#include "tchecker/publicapi/tck_liveness.hh"

int tck_liveness_default_block_size = 10000;
int tck_liveness_default_table_size = 65536;

static bool is_certificate_path(enum tck_liveness_certificate_t ctype) { return (ctype == CERTIFICATE_SYMBOLIC); }

const void tck_liveness_zg_ndfs(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl, const char * labels,
                                const char * search_order, int block_size, int table_size,
                                tck_liveness_certificate_t certificate)
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

const void tck_liveness_zg_couvscc(std::ostream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                                   const char * labels, const char * search_order, int block_size, int table_size,
                                   tck_liveness_certificate_t certificate)
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

const void tck_liveness(const char * output_filename, const char * sysdecl_filename, const char * labels,
                        tck_liveness_algorithm_t algorithm, tck_liveness_search_order_t search_order,
                        tck_liveness_certificate_t certificate, int * block_size, int * table_size)
{
  if (block_size == nullptr) {
    block_size = &tck_liveness_default_block_size;
  }

  if (table_size == nullptr) {
    table_size = &tck_liveness_default_table_size;
  }

  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    // create output stream to output file

    std::ostream * os = nullptr;
    if (strcmp(output_filename, "")) { // strcmp returns 0 (which is interpreted as false) if output_filename is "". 
      os = new std::ofstream(output_filename, std::ios::out);
    }
    else
      os = &std::cout;

    std::string search_order_str;
    if (search_order == BFS) {
      search_order_str = "bfs";
    }
    else if (search_order == DFS) {
      search_order_str = "dfs";
    }
    else {
      throw std::runtime_error("Unknown search order");
    }

    if (algorithm == ALGO_COUVSCC) {
      tck_liveness_zg_couvscc(*os, *sysdecl, labels, search_order_str.c_str(), *block_size, *table_size, certificate);
    }
    else if (algorithm == ALGO_NDFS) {
      tck_liveness_zg_ndfs(*os, *sysdecl, labels, search_order_str.c_str(), *block_size, *table_size, certificate);
    }
    else {
      throw std::runtime_error("Unknown algorithm");
    }
  }
  catch (std::exception const & e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
  catch (...) {
    std::cout << "Unknown error" << std::endl;
  }
}
