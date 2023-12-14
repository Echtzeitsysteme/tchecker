/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/system.hh"

#include <string>


namespace tchecker {

namespace strong_timed_bisim {

system_virtual_clocks_t::system_virtual_clocks_t(tchecker::ta::system_t const & system, std::size_t no_of_virtual_clocks, bool first_not_second)
: tchecker::ta::system_t(system),
_first_not_second(first_not_second), 
_no_of_virtual_clocks(no_of_virtual_clocks) {

  for(std::size_t i = 0; i < _no_of_virtual_clocks; ++i) {
    std::shared_ptr<std::string const> virtual_clock_name = std::make_shared<std::string const>(VIRTUAL_CLOCK_PREFIX + to_string(i));
    this->add_clock(*virtual_clock_name);
  }
}

tchecker::clock_id_t system_virtual_clocks_t::get_no_of_virtual_clocks() const
{
  return _no_of_virtual_clocks;
}


} // end of namespace strong_timed_bisim

} // end of namespace tchecker
