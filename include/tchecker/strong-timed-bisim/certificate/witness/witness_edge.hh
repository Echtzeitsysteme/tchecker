/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_EDGE_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_EDGE_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_edge.hh"
#include "tchecker/strong-timed-bisim/certificate/witness/witness_node.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

using edge_t = tchecker::strong_timed_bisim::certificate::edge_t<node_t>;

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
