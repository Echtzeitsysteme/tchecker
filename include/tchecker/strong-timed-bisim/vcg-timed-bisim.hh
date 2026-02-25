/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_COMPARE_VCG_TIMED_BISIM_HH
#define TCHECKER_TCK_COMPARE_VCG_TIMED_BISIM_HH

#include <memory>

#include "tchecker/parsing/parsing.hh"
#include "tchecker/strong-timed-bisim/stats.hh"

namespace tchecker {

namespace strong_timed_bisim {


/*!
 \brief Run check for timed bisimilarity
 \param sysdecl_first : first system declaration
 \param sysdecl_first : first system declaration
 \param block_size : number of elements allocated in one block
 \param table_size : size of hash tables
 \param first_starting_state_json : the json of the first starting state or the empty string, if init should be used
 \param second_starting_state_json : the json of the second starting state or the empty string, if init should be used
 \param generate_witness : whether a witness should be generated
 \post 
 \return statistics on the run and the reachability graph
 */
tchecker::strong_timed_bisim::stats_t
run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first, 
    std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second,
    std::ostream * os, std::size_t block_size, std::size_t table_size, 
    std::string first_starting_state_json, std::string second_starting_state_json, bool generate_witness);

} // end of namespace strong_timed_bisim
} // end of namespace tchecker

#endif // TCHECKER_TCK-COMPARE_VCG_TIMED_BISIM_HH
