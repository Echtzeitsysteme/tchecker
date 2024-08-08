/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_SEARCH_CONTRADICTION_HH
#define TCHECKER_STRONG_TIMED_BISIM_SEARCH_CONTRADICTION_HH

#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

/*!
 \brief finds a contradiction to timed bisimulation
 \param zone_A : the zone of the first symbolic state
 \param zone_B : the zone of the second symbolic state
 \param trans_A : the outgoing transitions of the first symbolic state
 \param trans_B : the outgoing transitions of the second symbolic state
 \param found_cont : the matrix where the already found contradictions of the outgoing transitions are stored
 */

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
search_contradiction(tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                     std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_A, std::vector<tchecker::vcg::vcg_t::sst_t *> & trans_B,
                     tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> found_cont, tchecker::clock_id_t no_of_virt_clks);

} // end of namespace strong_timed_bisim

} // end of namespace tchecker
#endif
