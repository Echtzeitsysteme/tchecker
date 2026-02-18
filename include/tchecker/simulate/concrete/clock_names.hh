/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_CLOCK_NAMES_HH
#define TCHECKER_TCK_SIMULATE_CLOCK_NAMES_HH

#include "tchecker/simulate/simulate.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/ta/ta.hh"


/*!
 \file concrete_graph.hh
 \brief Concrete simulation graph
*/

namespace tchecker {

namespace simulate {

namespace concrete {  


std::function<std::string (tchecker::clock_id_t)> clock_names(const tchecker::ta::system_t & system);

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace concrete

#endif