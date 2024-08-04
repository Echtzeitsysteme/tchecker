/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */


#ifndef TCHECKER_VCG_HH
#define TCHECKER_VCG_HH

#include "tchecker/strong-timed-bisim/system.hh"
#include "tchecker/zg/zg.hh"
#include "tchecker/zg/zone.hh"
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
   \note all states and transitions are pool allocated and deallocated automatically.
         enable_extrapolation is set to false, since the current bisimulation algorithm does the extrapolation by itself
   */
  vcg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
       std::shared_ptr<tchecker::zg::semantics_t> const & semantics, tchecker::clock_id_t no_of_virtual_clocks, bool urgent_or_committed,
       std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Accessor
   \return the number of virtual clocks
   */
  tchecker::clock_id_t get_no_of_virtual_clocks() const;

  /*!
   \brief Accessor
   \return whether urgent or committed locations exist
   */
  bool get_urgent_or_committed() const;

  inline tchecker::clock_id_t get_no_of_original_clocks() const { return _system->clocks_count(tchecker::variable_kind_t::VK_FLATTENED) - _no_of_virtual_clocks;}

  inline std::size_t get_no_of_locations() const { return _system->locations_count();}

private:

  tchecker::clock_id_t _no_of_virtual_clocks;
  bool _urgent_or_committed;
};

/*!
 \brief Factory of vcg with clock bounds computed from system
 \param extended_system : the system with virtual clocks
 \param first_not_second : true iff the extended system corresponds to the left hand side of the comparison
 \param orig_system_first : the left hand side of the comparison
 \param orig_system_second : the right hand side of the comparison
 \param urgent_or_committed : indicates whether the systems contain urgent or committed locations
 \param sharing_type : type of sharing
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation (currently, k_norm and local_m only!)
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
                               bool urgent_or_committed,
                               enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                               enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                               std::size_t table_size) ;

} // end of namespace vcg

} // end of namespace tchecker

#endif
