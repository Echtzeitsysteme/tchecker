/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/zone.hh"

namespace tchecker {

namespace vcg {

std::shared_ptr<virtual_constraint_t> zone_t::revert_action_trans(const tchecker::clock_constraint_container_t & guard,
                                                                  const tchecker::clock_reset_container_t & reset,
                                                                  const tchecker::clock_constraint_container_t & tgt_invariant,
                                                                  const virtual_constraint_t & phi_split)
{

  tchecker::clock_reset_container_t reset_copy;

  reset_copy.reserve(reset.size());

  std::copy(reset.begin(), reset.end(), reset_copy.begin());

  tchecker::dbm::db_t zone_clone[_dim*_dim];
  this->to_dbm(zone_clone);


  tchecker::dbm::db_t inter_zone[_dim*_dim];
  this->to_dbm(inter_zone);

  tchecker::dbm::constrain(inter_zone, _dim, guard);

  tchecker::dbm::db_t inter_clone[_dim*_dim];
  tchecker::dbm::copy(inter_clone, inter_zone, _dim);

  tchecker::dbm::reset(inter_zone, _dim, reset);
  tchecker::dbm::constrain(inter_zone, _dim, phi_split.get_vc());

  virtual_constraint_t *virt_mult_reset 
      = factory(tchecker::dbm::revert_multiple_reset(inter_clone, _dim, inter_zone, reset_copy), _dim, phi_split.get_no_of_virt_clocks());

  tchecker::dbm::constrain(zone_clone, _dim, virt_mult_reset->get_vc());

  std::shared_ptr<virtual_constraint_t> result = std::shared_ptr<virtual_constraint_t>(factory(zone_clone, _dim, phi_split.get_no_of_virt_clocks()));

  delete virt_mult_reset;

  return result;
}

std::shared_ptr<virtual_constraint_t> zone_t::revert_epsilon_trans(const virtual_constraint_t & phi_split)
{

  tchecker::dbm::db_t *D_split = phi_split.to_dbm();
  tchecker::dbm::open_down(D_split, _dim);

  tchecker::dbm::intersection(D_split, this->dbm(), D_split, _dim);

  std::shared_ptr<vcg::virtual_constraint_t> result = 
    std::shared_ptr<tchecker::vcg::virtual_constraint_t>(factory(D_split, _dim, phi_split.get_no_of_virt_clocks()));

  free(D_split);

  return result;
}

vcg::zone_t * factory(tchecker::clock_id_t dim)
{
  return reinterpret_cast<tchecker::vcg::zone_t *>(tchecker::zg::zone_allocate_and_construct(dim, dim));
}

vcg::zone_t * factory(tchecker::clock_id_t dim, zone_t const & zone)
{
  return reinterpret_cast<tchecker::vcg::zone_t *>(tchecker::zg::zone_allocate_and_construct(dim, zone));
}


} // end of namespace vcg

} // end of namespace tchecker
