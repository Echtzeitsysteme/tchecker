/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_SEARCH_CONTRADICTION_HH
#define TCHECKER_STRONG_TIMED_BISIM_SEARCH_CONTRADICTION_HH

#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/basictypes.hh"

namespace tchecker {

namespace strong_timed_bisim {

class contradiction_searcher_t {

  public:

    /*!
    \brief Constructor
    */
   contradiction_searcher_t(std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A, 
                            std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B,
                            tchecker::clock_id_t no_of_virt_clks);

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
                         std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A, 
                         std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B,
                         tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> found_cont);

    /*!
     \brief checks whether it makes sense to go on searching for contradictions
     \param zone_A : the zone of the first symbolic state
     \param zone_B : the zone of the second symbolic state
     \param trans_A : the outgoing transitions of the first symbolic state
     \param trans_B : the outgoing transitions of the second symbolic state
     \param found_cont : the matrix where the already found contradictions of the outgoing transitions are stored
     \param finished : the matrix which says at which pairs all contradictions are already found
    */
    bool 
    contradiction_still_possible(tchecker::zg::zone_t const & zone_A, tchecker::zg::zone_t const & zone_B,
                                 std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_A,
                                 std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans_B,
                                 tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> & found_cont,
                                 std::vector< std::vector<bool> > finished);
  private:

    tchecker::zone_matrix_t<tchecker::virtual_constraint::virtual_constraint_t> overhangs;
    tchecker::clock_id_t no_of_virt_clks;

    contradiction_searcher_t();

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> 
    find_contradiction(tchecker::zg::zone_t const & zone, std::shared_ptr<std::vector<tchecker::vcg::vcg_t::sst_t>> trans,
                       std::vector<std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> & found_con,
                       std::vector<std::shared_ptr<zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>> & cur_overhang);

};

} // end of namespace strong_timed_bisim

} // end of namespace tchecker
#endif
