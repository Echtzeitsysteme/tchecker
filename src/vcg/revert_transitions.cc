/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/revert_transitions.hh"


namespace tchecker {

namespace vcg {

std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
revert_action_trans(const tchecker::zg::zone_t & zone,
                    const tchecker::clock_constraint_container_t & guard,
                    const tchecker::clock_reset_container_t & reset,
                    const tchecker::clock_constraint_container_t & tgt_invariant,
                    const tchecker::virtual_constraint::virtual_constraint_t & phi_split)
{

  tchecker::clock_reset_container_t reset_copy;

  reset_copy.reserve(reset.size());

  std::copy(reset.begin(), reset.end(), reset_copy.begin());

  tchecker::dbm::db_t zone_clone[zone.dim()*zone.dim()];
  zone.to_dbm(zone_clone);


  tchecker::dbm::db_t d_land_g[zone.dim()*zone.dim()];
  zone.to_dbm(d_land_g);

  tchecker::dbm::constrain(d_land_g, zone.dim(), guard);


  tchecker::dbm::db_t r_d_land_g_land_phi[zone.dim()*zone.dim()];
  tchecker::dbm::copy(r_d_land_g_land_phi, d_land_g, zone.dim());

  tchecker::dbm::reset(r_d_land_g_land_phi, zone.dim(), reset);
  tchecker::dbm::constrain(r_d_land_g_land_phi, zone.dim(), phi_split.get_vc(zone.dim() - phi_split.dim()));

  tchecker::dbm::db_t *reverted = tchecker::dbm::revert_multiple_reset(d_land_g, zone.dim(), r_d_land_g_land_phi, reset_copy);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> virt_mult_reset 
      = tchecker::virtual_constraint::factory(reverted, zone.dim(), phi_split.get_no_of_virt_clocks());

  tchecker::dbm::constrain(zone_clone, zone.dim(), virt_mult_reset->get_vc(zone.dim() - phi_split.dim()));

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> result
      = tchecker::virtual_constraint::factory(zone_clone, zone.dim(), phi_split.get_no_of_virt_clocks());

  return result;
}

std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
revert_epsilon_trans(const tchecker::zg::zone_t & zone, const tchecker::virtual_constraint::virtual_constraint_t & phi_split)
{

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi_copy = factory(phi_split);

  tchecker::dbm::open_down(phi_copy->dbm(), phi_copy->dim());

  std::shared_ptr<tchecker::zg::zone_t> zone_copy = tchecker::zg::factory(zone);

  tchecker::dbm::constrain(zone_copy->dbm(), zone_copy->dim(), phi_copy->get_vc(zone_copy->dim() - phi_copy->dim()));

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> result
    = tchecker::virtual_constraint::factory(zone_copy, phi_split.get_no_of_virt_clocks());

  tchecker::zg::zone_destruct_and_deallocate(&(*phi_copy));
  tchecker::zg::zone_destruct_and_deallocate(&(*phi_copy));

  return result;
}

} // end of namespace vcg

} // end of namespace tchecker

