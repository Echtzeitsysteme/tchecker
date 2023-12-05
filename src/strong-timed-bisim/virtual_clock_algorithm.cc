/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/virtual_clock_algorithm.hh"

namespace tchecker {

namespace strong_timed_bisim {

tchecker::strong_timed_bisim::stats_t Lieb_et_al::run(std::shared_ptr<tchecker::zg::zg_t const> input_first, std::shared_ptr<tchecker::zg::zg_t const> input_second) {
  std::shared_ptr<tchecker::strong_timed_bisim::stats_t const> result = std::make_shared<tchecker::strong_timed_bisim::stats_t>();
  return *result;
}

} // enf of namespace strong_timed_bisim

} // end ofnamespace tchecker

