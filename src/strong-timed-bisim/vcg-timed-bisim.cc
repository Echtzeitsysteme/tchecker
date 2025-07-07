/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/vcg-timed-bisim.hh"

#include <string>
#include <memory>

#include "tchecker/system/static_analysis.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"
#include "tchecker/strong-timed-bisim/system.hh"
#include "tchecker/zg/zg.hh"

#define FIRST_PRODUCT_NAME "A"
#define SECOND_PRODUCT_NAME "B"

#define LOC_DELIMITER "__"

/* run */

namespace tchecker {


namespace strong_timed_bisim {

void check_for_init(std::shared_ptr<tchecker::system::system_t> const system){
  auto init_loc = system->initial_locations(0);
  auto iter = init_loc.begin();

  if (iter == init_loc.end()) {
    std::string error_message{"Currently, strong timed bisim is supported with exactly a single initial location per process, only\na process of "};
    error_message.append(system->name());
    error_message.append(" has no initial location");
    throw std::runtime_error(error_message);
  }

  ++iter;

  if (iter != init_loc.end()) {
    std::string error_message{"Currently, strong timed bisim is supported with exactly a single initial location per process, only\na process of "};
    error_message.append(system->name());
    error_message.append(" has more than a single initial location");
    throw std::runtime_error(error_message);
  }
}

tchecker::clock_id_t clocks_check(std::shared_ptr<tchecker::ta::system_t> const system) {
  auto p_c = system->clock_variables().identifiers(tchecker::VK_FLATTENED);
  for(auto it = p_c.begin(); it != p_c.end(); ++it) {
    std::string clock_name{system->clock_name(*it)};
    if (clock_name.rfind(VIRTUAL_CLOCK_PREFIX, 0) == 0) {
          std::string error_message{"The clock prefix "};
          error_message.append(VIRTUAL_CLOCK_PREFIX);
          error_message.append(" is reserved for virtual clocks and should not be used by the TA.");
          throw std::runtime_error(error_message);
    }
  }
  return system->clocks_count(VK_FLATTENED);
}

tchecker::strong_timed_bisim::stats_t
run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second,
    std::ostream * os, std::size_t block_size, std::size_t table_size) {

  std::vector<std::shared_ptr<tchecker::ta::system_t>> systems;

  for(size_t i = 0; i < 2; ++i) {
    std::shared_ptr<tchecker::ta::system_t> cur_system{new tchecker::ta::system_t{ (i == 0) ? *sysdecl_first : *sysdecl_second}};
    std::shared_ptr<tchecker::syncprod::system_t const> system_syncprod = std::make_shared<tchecker::syncprod::system_t const>(cur_system->as_syncprod_system());
    std::shared_ptr<tchecker::system::system_t> product = std::make_shared<tchecker::system::system_t>(tchecker::syncprod::synchronized_product(system_syncprod, (i == 0) ? FIRST_PRODUCT_NAME : SECOND_PRODUCT_NAME, LOC_DELIMITER));
    check_for_init(product);
    systems.push_back(cur_system);
  }

  tchecker::clock_id_t no_of_virt_clocks = clocks_check(systems[0]) + clocks_check(systems[1]);

  bool urgent_or_committed_clock = false;

  // if the systems contain urgent or committed locations, we need an extra clock
  if((systems[0]->committed_locations().count() + systems[0]->urgent_locations().count() + 
     systems[1]->committed_locations().count() + systems[1]->urgent_locations().count() )!= 0) {
    no_of_virt_clocks++;
    urgent_or_committed_clock = true;
  }

  std::vector<std::shared_ptr<tchecker::vcg::vcg_t>> vcgs;

  for(size_t i = 0; i < 2; ++i) {
    std::shared_ptr<tchecker::strong_timed_bisim::system_virtual_clocks_t const> extended_system{new tchecker::strong_timed_bisim::system_virtual_clocks_t{*(systems[i]), no_of_virt_clocks, 0 == i}};
    std::shared_ptr<tchecker::vcg::vcg_t> vcg{tchecker::vcg::factory(extended_system, 0 == i, systems[0], systems[1], urgent_or_committed_clock, tchecker::ts::SHARING, tchecker::zg::DISTINGUISHED_SEMANTICS,
                                                                     tchecker::zg::EXTRA_M_GLOBAL, block_size, table_size)};
    vcgs.push_back(vcg);
  }

//  std::cout << __FILE__ << ": " << __LINE__ << ": created vcgs" << std::endl;

  auto algorithm = new tchecker::strong_timed_bisim::Lieb_et_al(vcgs[0], vcgs[1]);

  return algorithm->run();

}

} // end of namespace strong_timed_bisim

} // end of namespace tchecker
