/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_SYSTEM_HH
#define TCHECKER_STRONG_TIMED_BISIM_SYSTEM_HH

#include "tchecker/ta/system.hh"

namespace tchecker {

namespace strong_timed_bisim {

class system_virtual_clocks_t : public tchecker::ta::system_t {

public:

  /*!
  \brief Constructor
  */
  system_virtual_clocks_t(tchecker::ta::system_t const & system, std::size_t no_of_virtual_clocks, bool first_not_second);

  /*!
  \brief Accessor
  \return the number of virtual clocks
  */
  tchecker::clock_id_t get_no_of_virtual_clocks() const;

private:
  // we disallow the standard constructor
  system_virtual_clocks_t();

  bool _first_not_second;
  tchecker::clock_id_t _no_of_virtual_clocks;

  using tchecker::ta::system_t::add_clock;

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif

