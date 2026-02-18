/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/simulate/concrete/clock_names.hh"

namespace tchecker {

namespace simulate {

namespace concrete {

std::function<std::string (tchecker::clock_id_t)> clock_names(const tchecker::ta::system_t & system) {
  auto result = [sys_ptr = &system](tchecker::clock_id_t id) {
    auto counter = id-1;
    auto const & clocks = sys_ptr->clock_variables();
    for(tchecker::clock_id_t base : clocks.identifiers(tchecker::VK_DECLARED)) {
      tchecker::clock_id_t size_of_cur_decl_clk = clocks.info(base).size();
      if(counter < size_of_cur_decl_clk) {
        if(1 == size_of_cur_decl_clk) {
          return clocks.name(base);
        }
        std::stringstream clk_name;
        clk_name << clocks.name(base) <<  "[" << counter << "]";
        return clk_name.str();
      }
      counter -= size_of_cur_decl_clk;
    }

    throw std::runtime_error(std::string(__FILE__) + std::string(": ") + std::to_string(__LINE__) + std::string(": ") +
                        std::string("strange clock id: ") + std::to_string(id));
  };

  return result;
}

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker