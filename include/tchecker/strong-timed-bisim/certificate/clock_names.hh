/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CLOCK_NAMES_HH
#define TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CLOCK_NAMES_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_node.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace certificate {

std::function<std::string (tchecker::clock_id_t)> 
clock_names(const std::shared_ptr<tchecker::vcg::vcg_t> vcg1, const std::shared_ptr<tchecker::vcg::vcg_t> vcg2);


std::function<std::string (tchecker::clock_id_t)> 
clock_names(const std::shared_ptr<tchecker::vcg::vcg_t> vcg, std::string postfix);
}

}

}

#endif