#include "tchecker/algorithms/concur19/concur19.hh"
#include "tchecker/algorithms/covreach/zg-covreach.hh"
#include "tchecker/algorithms/reach/zg-reach.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/system/system.hh"

#include "tchecker/publicapi/tck_reach.hh"

int tck_reach_default_block_size = 10000;
int tck_reach_default_table_size = 65536;

static bool is_certificate_path(enum tck_reach_certificate_t ctype)
{
  return (ctype == CERTIFICATE_SYMBOLIC || ctype == CERTIFICATE_CONCRETE);
}

const void tck_reach_zg_reach(std::ostringstream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                              const char * labels, const char * search_order, int block_size, int table_size,
                              tck_reach_certificate_t certificate)
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

const void tck_reach_zg_covreach(std::ostringstream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                                 const char * labels, const char * search_order, int block_size, int table_size,
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

const void tck_reack_concur19(std::ostringstream & os, const tchecker::parsing::system_declaration_t & sysdecl,
                              const char * labels, const char * search_order, int block_size, int table_size,
                              tck_reach_certificate_t certificate)
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

const char * tck_reach(const char * sysdecl_filename, tck_reach_algorithm_t algorithm, tck_reach_search_order_t search_order,
                       tck_reach_certificate_t certificate, int * block_size, int * table_size)
{
  if (block_size == nullptr) {
    block_size = &tck_reach_default_block_size;
  }

  if (table_size == nullptr) {
    table_size = &tck_reach_default_table_size;
  }

  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(*sysdecl));

    std::ostringstream oss;

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

    if (algorithm == ALGO_REACH) {
      tck_reach_zg_reach(oss, *sysdecl, "", search_order_str.c_str(), *block_size, *table_size, certificate);
    }
    else if (algorithm == ALGO_CONCUR19) {
      tck_reach_zg_covreach(oss, *sysdecl, "", search_order_str.c_str(), *block_size, *table_size, certificate);
    }
    else if (algorithm == ALGO_COVREACH) {
      tck_reach_zg_covreach(oss, *sysdecl, "", search_order_str.c_str(), *block_size, *table_size, certificate);
    }
    else {
      throw std::runtime_error("Unknown algorithm");
    }

    std::string result = oss.str();

    char * c_output = (char *)std::malloc(result.size() + 1);
    if (c_output == nullptr) {
      throw std::bad_alloc();
    }

    std::strcpy(c_output, result.c_str());
    return c_output;
  }
  catch (std::exception const & e) {
    return e.what();
  }
}
