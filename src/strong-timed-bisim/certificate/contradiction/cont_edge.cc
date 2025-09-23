/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/contradiction/cont_edge.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace contra {

delay_edge_t::delay_edge_t(clock_rational_value_t delay, std::shared_ptr<node_t> source, std::shared_ptr<node_t> target) :
                           _delay(delay), _src(source), _tgt(target)
{
}

void delay_edge_t::attributes(std::map<std::string, std::string> & m) const
{
  m["delay"] = to_string(_delay);
}


} // namespace contra

} // namespace strong_timed_bisim

} // namespace tchecker