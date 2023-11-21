/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "vcg-timed-bisim.hh"

#include <string>

#include "tchecker/system/static_analysis.hh"
#include "tchecker/ta/system.hh"

/* run */

tchecker::algorithms::compare::stats_t
tchecker::tck_compare::vcg_timed_bisim::run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second,
    std::ostream * os, std::size_t block_size, std::size_t table_size) {

  std::shared_ptr<tchecker::ta::system_t const> system_first{new tchecker::ta::system_t{*sysdecl_first}};
  std::shared_ptr<tchecker::ta::system_t const> system_second{new tchecker::ta::system_t{*sysdecl_second}};

  auto check_for_init = [](std::shared_ptr<tchecker::ta::system_t const> const system, std::string name){
    if (!tchecker::system::every_process_has_initial_location(system->as_system_system()))
      std::cerr << tchecker::log_warning << name << " has no initial state" << std::endl;
  };

  check_for_init(system_first, "the first system");
  check_for_init(system_second, "the second system");

  std::cout << "RUN!" << std::endl;
  auto result = new tchecker::algorithms::compare::stats_t();
  return *result;
}
