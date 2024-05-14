/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/revert_transitions.hh"


namespace tchecker {

namespace vcg {

bool check_whether_phi_is_subset_of_target(
                    const tchecker::zg::zone_t & zone,
                    const tchecker::clock_constraint_container_t & guard,
                    const tchecker::clock_reset_container_t & reset,
                    const tchecker::clock_constraint_container_t & tgt_invariant,
                    const tchecker::virtual_constraint::virtual_constraint_t & phi_split)
{
  tchecker::dbm::db_t target_dbm[zone.dim()*zone.dim()];
  zone.to_dbm(target_dbm);

  tchecker::dbm::constrain(target_dbm, zone.dim(), guard);

  if(!tchecker::dbm::is_tight(target_dbm, zone.dim())) {
    std::cerr << __FILE__ << ": " << __LINE__ << ": target_dbm is not tight" << std::endl;
    return false;
  }

  if(!tchecker::dbm::is_consistent(target_dbm, zone.dim())) {
    std::cerr << __FILE__ << ": " << __LINE__ << ": target_dbm is not consistent" << std::endl;
    return false;
  }

  tchecker::dbm::reset(target_dbm, zone.dim(), reset);
  tchecker::dbm::constrain(target_dbm, zone.dim(), tgt_invariant);

  if(!tchecker::dbm::is_tight(target_dbm, zone.dim())) {
    std::cerr << __FILE__ << ": " << __LINE__ << ": target_dbm is not tight" << std::endl;
    return false;
  }

  if(!tchecker::dbm::is_consistent(target_dbm, zone.dim())) {
    std::cerr << __FILE__ << ": " << __LINE__ << ": target_dbm is not consistent" << std::endl;
    return false;
  }

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> target_vc 
    = tchecker::virtual_constraint::factory(target_dbm, zone.dim(), phi_split.get_no_of_virt_clocks());

  return tchecker::dbm::is_le(phi_split.dbm(), target_vc->dbm(), target_vc->dim());

}

std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
revert_action_trans(const tchecker::zg::zone_t & zone,
                    const tchecker::clock_constraint_container_t & guard,
                    const tchecker::clock_reset_container_t & reset,
                    const tchecker::clock_constraint_container_t & tgt_invariant,
                    const tchecker::virtual_constraint::virtual_constraint_t & phi_split)
{
  assert(!zone.is_empty());
  assert(!phi_split.is_empty());

  assert(tchecker::dbm::is_tight(zone.dbm(), zone.dim()));
  assert(tchecker::dbm::is_consistent(zone.dbm(), zone.dim()));

  assert(tchecker::dbm::is_tight(phi_split.dbm(), phi_split.dim()));
  assert(tchecker::dbm::is_consistent(phi_split.dbm(), phi_split.dim()));

  assert(check_whether_phi_is_subset_of_target(zone, guard, reset, tgt_invariant, phi_split));

  // copy the reset container since revert_multiple_reset will change it
  tchecker::clock_reset_container_t reset_copy(reset);

  // According to the implementation of revert-action-trans, described in Lieb et al., we first have to calculate R(zone && g) && phi_split.
  tchecker::dbm::db_t d_land_g[zone.dim()*zone.dim()];
  zone.to_dbm(d_land_g);

  assert(tchecker::dbm::is_tight(d_land_g, zone.dim()));
  assert(tchecker::dbm::is_consistent(d_land_g, zone.dim()));

  tchecker::dbm::constrain(d_land_g, zone.dim(), guard);

  tchecker::dbm::db_t r_d_land_g_land_phi[zone.dim()*zone.dim()];
  tchecker::dbm::copy(r_d_land_g_land_phi, d_land_g, zone.dim());

  tchecker::dbm::reset(r_d_land_g_land_phi, zone.dim(), reset);

  assert(tchecker::dbm::is_tight(r_d_land_g_land_phi, zone.dim()));
  assert(tchecker::dbm::is_consistent(r_d_land_g_land_phi, zone.dim()));

  // now we have calculated R(zone && g) and we have to land this with the given virtual constrain
  tchecker::dbm::constrain(r_d_land_g_land_phi, zone.dim(), phi_split.get_vc(zone.dim() - phi_split.dim(), true));

  assert(tchecker::dbm::is_tight(r_d_land_g_land_phi, zone.dim()));
  assert(tchecker::dbm::is_consistent(r_d_land_g_land_phi, zone.dim()));

  tchecker::dbm::db_t *reverted = (tchecker::dbm::db_t *)malloc(zone.dim()*zone.dim()*sizeof(tchecker::dbm::db_t));

  tchecker::dbm::revert_multiple_reset(reverted, d_land_g, zone.dim(), r_d_land_g_land_phi, reset_copy);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> virt_mult_reset 
      = tchecker::virtual_constraint::factory(reverted, zone.dim(), phi_split.get_no_of_virt_clocks());

  return virt_mult_reset;
}

std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
revert_epsilon_trans(const tchecker::zg::zone_t & zone, const tchecker::zg::zone_t & zone_eps, const tchecker::virtual_constraint::virtual_constraint_t & phi_split)
{

  std::shared_ptr<tchecker::zg::zone_t> zone_eps_copy = tchecker::zg::factory(zone_eps);

  if(tchecker::dbm::EMPTY == tchecker::dbm::constrain(zone_eps_copy->dbm(), zone_eps_copy->dim(), phi_split.get_vc(zone_eps_copy->dim() - phi_split.dim(), true))) {
    //tchecker::dbm::tighten(zone_eps_copy->dbm(), zone_eps_copy->dim());
    // std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> result = tchecker::virtual_constraint::factory(zone_eps_copy->dbm(), zone_eps_copy->dim(), phi_split.dim() - 1);
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> result = tchecker::virtual_constraint::factory(phi_split.dim() - 1);
    tchecker::dbm::empty(result->dbm(), result->dim());
    return result;
  }

  tchecker::dbm::open_down(zone_eps_copy->dbm(), zone_eps_copy->dim());

  std::shared_ptr<tchecker::zg::zone_t> zone_copy = tchecker::zg::factory(zone);

  intersection(zone_copy->dbm(), zone_copy->dbm(), zone_eps_copy->dbm(), zone_eps_copy->dim());

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> result
    = tchecker::virtual_constraint::factory(zone_copy, phi_split.get_no_of_virt_clocks());

  return result;
}

} // end of namespace vcg

} // end of namespace tchecker

