/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "vcg-timed-bisim.hh"

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

namespace tck_compare {

namespace vcg_timed_bisim {

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
  std::size_t result = 0;
  auto p_c = system->clock_variables().identifiers();
  auto begin_p_c = p_c.begin(), end_p_c = p_c.end();
  for(auto it = begin_p_c; it != end_p_c; ++it) {
    std::string clock_name{system->clock_name(it)};
    if (clock_name.rfind(VIRTUAL_CLOCK_PREFIX, 0) == 0) {
          std::string error_message{"The clock prefix "};
          error_message.append(VIRTUAL_CLOCK_PREFIX);
          error_message.append(" is reserved for virtual clocks and should not be used by the TA.");
          throw std::runtime_error(error_message);
    }
    ++result;
  }
  return system->clocks_count(VK_FLATTENED);
}

tchecker::strong_timed_bisim::stats_t
run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second,
    std::ostream * os, std::size_t block_size, std::size_t table_size) {

  std::vector<std::shared_ptr<tchecker::ta::system_t>> systems;

  std::cout << "vcg-timed-bisim.cc : run" << std::endl;

  for(size_t i = 0; i < 2; ++i) {
    std::shared_ptr<tchecker::ta::system_t> cur_system{new tchecker::ta::system_t{ (i == 0) ? *sysdecl_first : *sysdecl_second}};
    std::shared_ptr<tchecker::syncprod::system_t const> system_syncprod = std::make_shared<tchecker::syncprod::system_t const>(cur_system->as_syncprod_system());
    std::shared_ptr<tchecker::system::system_t> product = std::make_shared<tchecker::system::system_t>(tchecker::syncprod::synchronized_product(system_syncprod, (i == 0) ? FIRST_PRODUCT_NAME : SECOND_PRODUCT_NAME, LOC_DELIMITER));
    check_for_init(product);
    systems.push_back(cur_system);
  }

  tchecker::clock_id_t no_of_virt_clocks = clocks_check(systems[0]) + clocks_check(systems[1]);

  std::vector<std::shared_ptr<tchecker::vcg::vcg_t>> vcgs;

  for(size_t i = 0; i < 2; ++i) {
    std::shared_ptr<tchecker::strong_timed_bisim::system_virtual_clocks_t const> extended_system{new tchecker::strong_timed_bisim::system_virtual_clocks_t{*(systems[i]), no_of_virt_clocks, 0 == i}};
    std::shared_ptr<tchecker::vcg::vcg_t> vcg{tchecker::vcg::factory(extended_system, 0 == i, systems[0], systems[1], tchecker::ts::SHARING, tchecker::zg::DISTINGUISHED_SEMANTICS,
                                                               tchecker::zg::EXTRA_K_NORM, block_size, table_size)};
    vcgs.push_back(vcg);
  }

  std::cout << "vcg-timed-bisim.cc : created vcgs" << std::endl;

  auto algorithm = new tchecker::strong_timed_bisim::Lieb_et_al();

  return algorithm->run(vcgs[0], vcgs[1]);


  /* DELETE ME!

  std::cout << "no. of processes: " << system_first->processes_count() << std::endl;
  std::cout << "no. of processes of product: " << product_first->processes_count() << std::endl;

  auto r = system_first->attributes().range();
  auto begin = r.begin(), end = r.end();

  std::cout << "loop system attributes" << std::endl;

  for(auto it = begin; it != end; ++it) {
    std::cout << "found attribute: " << (*it).key() << ":" << (*it).value() << std::endl;
  }

  auto p = product_first->attributes().range();
  auto pbegin = p.begin(), pend = p.end();

  std::cout << "loop product attributes" << std::endl;

  for(auto it = pbegin; it != pend; ++it) {
    std::cout << "found attribute: " << (*it).key() << ":" << (*it).value() << std::endl;
  }

  auto r_l = system_first->locations();
  auto begin_l = r_l.begin(), end_l = r_l.end();

  std::cout << "loop system locations" << std::endl;

  for(auto it = begin_l; it != end_l; ++it) {
    std::cout << "found location: " << (*it)->name() << std::endl;
  }

  auto p_l = product_first->locations();
  auto begin_p_l = p_l.begin(), end_p_l = p_l.end();

  std::cout << "loop product locations" << std::endl;

  for(auto it = begin_p_l; it != end_p_l; ++it) {
    std::cout << "found location: " << (*it)->name() << std::endl;
  }

  std::cout << "initial system locations" << std::endl;

  for(std::size_t i = 0; i < system_first->processes_count(); ++i) {
      auto r_init = system_first->initial_locations(i);
      auto begin_init = r_init.begin(), end_init = r_init.end();

    for(auto it = begin_init; it != end_init; ++it) {
      std::cout << "found initial location for process " << i << "(" << system_first->process_name(i) << "): " << (*it)->name() << std::endl;
    }
  }

  std::cout << "initial product locations" << std::endl;

  auto p_init = product_first->initial_locations(0);
  auto begin_init = p_init.begin(), end_init = p_init.end();

  for(auto it = begin_init; it != end_init; ++it) {
      std::cout << "found initial location for process " << 0 << "(" << product_first->process_name(0) << "): " << (*it)->name() << std::endl;
  }

  auto r_c = system_first->clock_variables().identifiers();
  auto begin_c = r_c.begin(), end_c = r_c.end();

  std::cout << "loop system clocks" << std::endl;

  for(auto it = begin_c; it != end_c; ++it) {
    std::cout << "found clock: " << it << ": " << system_first->clock_name(it) << std::endl;
  }

  auto p_c = product_first->clock_variables().identifiers();
  auto begin_p_c = p_c.begin(), end_p_c = p_c.end();

  std::cout << "loop product clocks" << std::endl;

    for(auto it = begin_p_c; it != end_p_c; ++it) {
    std::cout << "found clock: " << it << ": " << product_first->clock_name(it) << std::endl;
  }


  /* UNTIL ME! */

}

} // end of namespace vcg_timed_bisim

} // end of namespace tck_compare

} // end of namespace tchecker
