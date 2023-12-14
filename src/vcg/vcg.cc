/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace vcg {

vcg_t::vcg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
              std::shared_ptr<tchecker::zg::semantics_t> const & semantics, tchecker::clock_id_t no_of_virtual_clocks,
              std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size)
  : tchecker::zg::zg_t(system, sharing_type, semantics, extrapolation, block_size, table_size),
    _no_of_virtual_clocks(no_of_virtual_clocks)
{
}

tchecker::clock_id_t vcg_t::get_no_of_virtual_clocks() const
{
  return _no_of_virtual_clocks;
}

tchecker::dbm::db_t * vcg_t::revert_multiple_reset(tchecker::dbm::db_t * orig_zone, tchecker::clock_id_t dim, tchecker::dbm::db_t * zone_split, tchecker::clock_reset_container_t reset)
{
  if(reset.empty()) {
    return zone_split;
  }

  tchecker::dbm::db_t zone_clone[dim*dim];
  tchecker::dbm::copy(zone_clone, orig_zone, dim);

  tchecker::clock_reset_t cur = reset.back();
  reset.pop_back();

  tchecker::dbm::reset(zone_clone, dim, reset);

  tchecker::dbm::free_clock(zone_split, dim, cur);

  tchecker::dbm::db_t new_split[dim*dim];

  intersection(new_split, zone_clone, zone_split, dim);

  return revert_multiple_reset(orig_zone, dim, new_split, reset);

}

std::shared_ptr<virtual_constraint_t> vcg_t::revert_action_trans(const tchecker::dbm::db_t * zone, tchecker::clock_id_t dim, const tchecker::clock_constraint_container_t & guard,
                                              const tchecker::clock_reset_container_t & reset, const tchecker::clock_constraint_container_t & tgt_invariant,
                                              const virtual_constraint_t & phi_split)
{

  tchecker::clock_reset_container_t reset_copy;

  reset_copy.reserve(reset.size());

  std::copy(reset.begin(), reset.end(), reset_copy.begin());

  // TODO: it is crazy what is happening here. I have no idea how I shall instantiate a new zone. I just run
  // all the operations that are supposed to run over zones over DBMs, which  means that I have to use the wrong abstraction layer here.
  // This whole allocator stuff really needs to get fixed ASAP. I am completely unable to use it properly and
  // there is a reason why the standard lib exists. Using it instead of creating own stuff would be great...

  tchecker::dbm::db_t zone_clone[dim*dim];
  tchecker::dbm::copy(zone_clone, zone, dim);


  tchecker::dbm::db_t inter_zone[dim*dim];
  tchecker::dbm::copy(inter_zone, zone, dim);
  tchecker::dbm::constrain(inter_zone, dim, guard);

  tchecker::dbm::db_t inter_clone[dim*dim];
  tchecker::dbm::copy(inter_clone, inter_zone, dim);

  tchecker::dbm::reset(inter_zone, dim, reset);
  tchecker::dbm::constrain(inter_zone, dim, phi_split.get_vc());

  virtual_constraint_t *virt_mult_reset = factory(revert_multiple_reset(inter_clone, dim, inter_zone, reset_copy), dim, _no_of_virtual_clocks);

  tchecker::dbm::constrain(zone_clone, dim, virt_mult_reset->get_vc());

  std::shared_ptr<virtual_constraint_t> result = std::shared_ptr<virtual_constraint_t>(factory(zone_clone, dim, _no_of_virtual_clocks));

  delete virt_mult_reset;

  return result;
}

tchecker::vcg::vcg_t * factory(std::shared_ptr<tchecker::strong_timed_bisim::system_virtual_clocks_t const> const & extended_system,
                               bool first_not_second,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_first,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_second,
                               enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                               enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                               std::size_t table_size) 
{

  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
    tchecker::vcg::extrapolation_factory(extrapolation_type, orig_system_first, orig_system_second, first_not_second)};

  if (extrapolation.get() == nullptr)
    return nullptr;

  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};

  tchecker::clock_id_t vc = extended_system->get_no_of_virtual_clocks();

  return new tchecker::vcg::vcg_t(extended_system, sharing_type, semantics, vc, extrapolation, block_size, table_size);

}

} // end of namespace vcg

} // end of namespace tchecker
