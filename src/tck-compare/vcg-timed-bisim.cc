/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "vcg-timed-bisim.hh"

/* run */

tchecker::algorithms::compare::stats_t
tchecker::tck_compare::vcg_timed_bisim::run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second,
    std::ostream * os, std::size_t block_size, std::size_t table_size) {
  std::cout << "RUN!" << std::endl;
  auto result = new tchecker::algorithms::compare::stats_t();
  return *result;
}
