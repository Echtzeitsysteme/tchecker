/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */


#ifndef TCHECKER_VCG_HH
#define TCHECKER_VCG_HH

#include "tchecker/zg/zg.hh"
#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/strong-timed-bisim/system.hh"
#include "tchecker/vcg/virtual_constraint.hh"

/*!
 \file vcg.hh
 \brief Virtual Clock Graphs
 */

namespace tchecker {

namespace vcg {

/*!
 \class vcg_t
 \brief Transition system of the virtual clock graph over system of timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically. A vcg is a zg
 * with some extensions.
 */
class vcg_t : public tchecker::zg::zg_t {
public:
  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param sharing_type : type of state/transition sharing
   \param semantics : a zone semantics
   \param no_of_virtual_clocks : number of virtual clocks
   \param extrapolation : a zone extrapolation 
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated automatically
   */
  vcg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
       std::shared_ptr<tchecker::zg::semantics_t> const & semantics, tchecker::clock_id_t no_of_virtual_clocks,
       std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size);

  /*!
  \brief Accessor
  \return the number of virtual clocks
  */
  tchecker::clock_id_t get_no_of_virtual_clocks() const;

  /*!
  \brief revert-action-trans function (see the TR of Lieb et al.)
  \param zone : the original zone
  \param dim : dimension of zone
  \param guard : of the transition to revert
  \param reset : the reset set of the transition to revert
  \param tgt_invariant : the invariant of the target state of the transition to revert
  \param phi_split : the sub vc of the target
  \return a shared pointer to the resulting virtual constraint
  */
  std::shared_ptr<virtual_constraint_t> revert_action_trans(const tchecker::dbm::db_t * zone, tchecker::clock_id_t dim, const tchecker::clock_constraint_container_t & guard,
                                                const tchecker::clock_reset_container_t & reset, const tchecker::clock_constraint_container_t & tgt_invariant,
                                                const virtual_constraint_t & phi_split);

  /*!
  \brief revert-epsilon-trans function (see the TR of Lieb et al.)
  \param zone : the original zone
  \param phi_split : the sub vc of the target
  \return a shared pointer to the resulting virtual constraint
  */
  //std::shared_ptr<virtual_constraint_t> revert_epsilon_trans(const tchecker::dbm::db_t * zone, const virtual_constraint_t & phi_split);

private:

 /*!
  \brief revert-multiple-reset function (see the TR of Lieb et al.)
  \param orig_zone : the original zone
  \oaram dim : dimension of orig_zone
  \param zone_split : the split of reset(orig_zone)
  \param reset : the used reset set
  \return the zone with reverted resets (same dim as orig_zone)
  */
tchecker::dbm::db_t * revert_multiple_reset(tchecker::dbm::db_t * orig_zone, tchecker::clock_id_t dim, tchecker::dbm::db_t * zone_split, tchecker::clock_reset_container_t reset);

  tchecker::clock_id_t _no_of_virtual_clocks;
};

/*!
 \brief Factory of vcg with clock bounds computed from system
 \param extended_system : the system with virtual clocks
 \param first_not_second : true iff the extended system corresponds to the left hand side of the comparison
 \param orig_system_first : the left hand side of the comparison
 \param orig_system_second : the right hand side of the comparison
 \param sharing_type : type of sharing
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation (currently, k_norm only!)
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a vcg over system with zone semantics and zone extrapolation
 defined from semantics_type and extrapolation_type, and allocation of
 block_size objects at a time, nullptr if clock bounds cannot be inferred from
 system
 */
tchecker::vcg::vcg_t * factory(std::shared_ptr<tchecker::strong_timed_bisim::system_virtual_clocks_t const> const & extended_system,
                               bool first_not_second,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_first,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_second,
                               enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                               enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             std::size_t table_size) ;

} // end of namespace vcg

} // end of namespace tchecker

#endif
