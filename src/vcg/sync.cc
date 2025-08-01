/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/sync.hh"

namespace tchecker {

namespace vcg {

bool is_virtually_equivalent(tchecker::dbm::db_t const *dbm1, tchecker::dbm::db_t const *dbm2,
                                tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
                                tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2)
{

  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);

  assert(dim1 >= 1);
  assert(dim2 >= 1);

  assert(tchecker::dbm::is_consistent(dbm1, dim1));
  assert(tchecker::dbm::is_consistent(dbm2, dim2));
  assert(tchecker::dbm::is_tight(dbm1, dim1));
  assert(tchecker::dbm::is_tight(dbm2, dim2));

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc1 
    = tchecker::virtual_constraint::factory(dbm1, dim1, dim1 - no_of_orig_clocks_1 - 1);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc2 
    = tchecker::virtual_constraint::factory(dbm2, dim2, dim2 - no_of_orig_clocks_2 - 1);

  return (*vc1 == *vc2);
}


bool are_dbm_synced(tchecker::dbm::db_t const *dbm1, tchecker::dbm::db_t const *dbm2,
                    tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
                    tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2)
{

  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);

  assert(dim1 >= 1);
  assert(dim2 >= 1);

  assert(tchecker::dbm::is_consistent(dbm1, dim1));
  assert(tchecker::dbm::is_consistent(dbm2, dim2));
  assert(tchecker::dbm::is_tight(dbm1, dim1));
  assert(tchecker::dbm::is_tight(dbm2, dim2));

  assert(no_of_orig_clocks_1 >= 1);
  assert(no_of_orig_clocks_2 >= 1);

  bool result = is_virtually_equivalent(dbm1, dbm2, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2);

  if(!result) {
    return result;
  }

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_1; ++i) {
    tchecker::dbm::db_t * orig_min_virt = tchecker::dbm::access(const_cast<tchecker::dbm::db_t *>(dbm1), dim1, i, i + no_of_orig_clocks_1);
    tchecker::dbm::db_t * virt_min_orig = tchecker::dbm::access(const_cast<tchecker::dbm::db_t *>(dbm1), dim1, i + no_of_orig_clocks_1, i);

    result &= (*orig_min_virt == *virt_min_orig);
    result &= (orig_min_virt->cmp == tchecker::LE);
    result &= (orig_min_virt->value == 0);
    if(!result) {
      return result;
    }
  }

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_2; ++i) {
    tchecker::dbm::db_t * orig_min_virt = tchecker::dbm::access(const_cast<tchecker::dbm::db_t *>(dbm2), dim2, i, i + no_of_orig_clocks_1 + no_of_orig_clocks_2);
    tchecker::dbm::db_t * virt_min_orig = tchecker::dbm::access(const_cast<tchecker::dbm::db_t *>(dbm2), dim2, i + no_of_orig_clocks_1 + no_of_orig_clocks_2, i);

    result &= (*orig_min_virt == *virt_min_orig);
    result &= (orig_min_virt->cmp == tchecker::LE);
    result &=(orig_min_virt->value == 0);
    if(!result) {
      return result;
    }
  }

  return result;
}

bool are_zones_synced(tchecker::zg::zone_t const & zone_1, tchecker::zg::zone_t const & zone_2,
                      tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2)
{
  return are_dbm_synced(zone_1.dbm(), zone_2.dbm(), zone_1.dim(), zone_2.dim(), no_of_orig_clocks_1, no_of_orig_clocks_2);
}

bool is_phi_subset_of_a_zone(const tchecker::dbm::db_t *dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_orig_clocks,
                             const tchecker::virtual_constraint::virtual_constraint_t & phi_e)
{

  assert(phi_e.dim() == dim - no_of_orig_clocks);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi = tchecker::virtual_constraint::factory(dbm, dim, dim - no_of_orig_clocks - 1);

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  assert(tchecker::dbm::is_consistent(phi->dbm(), phi->dim()));
  assert(tchecker::dbm::is_tight(phi->dbm(), phi->dim()));

  if(!tchecker::dbm::is_le(phi_e.dbm(), phi->dbm(), phi_e.dim())) {
    std::cout << __FILE__ << ": " << __LINE__ << ": phi of zone:" << std::endl;
    tchecker::dbm::output_matrix(std::cout, phi->dbm(), phi->dim());

    std::cout << __FILE__ << ": " << __LINE__ << ": phi given:" << std::endl;
    tchecker::dbm::output_matrix(std::cout, phi_e.dbm(), phi_e.dim());

    return false;

  }

  return true;
}

void sync(tchecker::dbm::db_t *dbm1, tchecker::dbm::db_t *dbm2,
          tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
          tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
          tchecker::clock_reset_container_t const & orig_reset1,
          tchecker::clock_reset_container_t const & orig_reset2)
{
  assert(is_virtually_equivalent(dbm1, dbm2, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2));

  if(are_dbm_synced(dbm1, dbm2, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2)) {
    return;
  }

  for(const tchecker::clock_reset_t & r : orig_reset1) {

    if(r.right_id() != tchecker::REFCLOCK_ID || r.value() != 0) {
      throw std::runtime_error("when checking for timed bisim, only resets to value zero are allowed");
    }
    reset_to_value(dbm1, dim1, r.left_id() + 1 + no_of_orig_clocks_1, 0);
    reset_to_value(dbm2, dim2, r.left_id() + 1 + no_of_orig_clocks_2, 0);
  }

  for(const tchecker::clock_reset_t & r : orig_reset2) {

    if(r.right_id() != tchecker::REFCLOCK_ID || r.value() != 0) {
      throw std::runtime_error("when checking for timed bisim, only resets to value zero are allowed");
    }
    reset_to_value(dbm1, dim1, r.left_id() + 1 + no_of_orig_clocks_1 + no_of_orig_clocks_1, 0);
    reset_to_value(dbm2, dim2, r.left_id() + 1 + no_of_orig_clocks_2 + no_of_orig_clocks_1, 0);
  }

  assert(are_dbm_synced(dbm1, dbm2, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2));
}

void sync(tchecker::zg::zone_t & zone_1, tchecker::zg::zone_t & zone_2,
          tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
          tchecker::clock_reset_container_t const & orig_reset1,
          tchecker::clock_reset_container_t const & orig_reset2)
{
  sync(zone_1.dbm(), zone_2.dbm(), zone_1.dim(), zone_2.dim(), no_of_orig_clocks_1, no_of_orig_clocks_2, orig_reset1, orig_reset2);
}

std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>>
revert_sync(tchecker::zg::zone_t const & zone_1, tchecker::zg::zone_t const & zone_2,
            tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
            const tchecker::virtual_constraint::virtual_constraint_t & phi_e)
{

  auto dim1 = zone_1.dim();
  tchecker::dbm::db_t dbm1[dim1*dim1];
  tchecker::dbm::copy(dbm1, zone_1.dbm(), zone_1.dim());

  auto dim2 = zone_2.dim();
  tchecker::dbm::db_t dbm2[dim2*dim2];
  tchecker::dbm::copy(dbm2, zone_2.dbm(), zone_2.dim());

  assert(tchecker::dbm::is_consistent(dbm1, dim1));
  assert(tchecker::dbm::is_consistent(dbm2, dim2));
  assert(tchecker::dbm::is_tight(dbm1, dim1));
  assert(tchecker::dbm::is_tight(dbm2, dim2));

  assert(is_virtually_equivalent(const_cast<tchecker::dbm::db_t *>(dbm1), const_cast<tchecker::dbm::db_t *>(dbm2), dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2));

  // due to the fact that the number of original clocks might differ, we need to build two reset sets
  tchecker::clock_reset_container_t orig_reset_set_A;
  tchecker::clock_reset_container_t orig_reset_set_B;

  tchecker::clock_reset_container_t virt_reset_set_A;
  tchecker::clock_reset_container_t virt_reset_set_B;

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_1; ++i) {
    if(tchecker::dbm::LE_ZERO != *tchecker::dbm::access(dbm1, dim1, i, no_of_orig_clocks_1 + i) || tchecker::dbm::LE_ZERO != *tchecker::dbm::access(dbm1, dim1, no_of_orig_clocks_1 + i, i)) {

        tchecker::clock_reset_t orig_tmp{i-1, tchecker::REFCLOCK_ID, 0};

        tchecker::clock_reset_t virt_tmp_A{i + no_of_orig_clocks_1 - 1, tchecker::REFCLOCK_ID, 0};
        tchecker::clock_reset_t virt_tmp_B{i + no_of_orig_clocks_2 - 1, tchecker::REFCLOCK_ID, 0};

        orig_reset_set_A.emplace_back(orig_tmp);

        virt_reset_set_A.emplace_back(virt_tmp_A);
        virt_reset_set_B.emplace_back(virt_tmp_B);
    }
  }

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_2; ++i) {
    if(tchecker::dbm::LE_ZERO != *tchecker::dbm::access(dbm2, dim2, i, no_of_orig_clocks_1 + no_of_orig_clocks_2 + i) || tchecker::dbm::LE_ZERO != *tchecker::dbm::access(dbm2, dim2, no_of_orig_clocks_1 + no_of_orig_clocks_2 + i, i)) {

        tchecker::clock_reset_t orig_tmp{i -1, tchecker::REFCLOCK_ID, 0};

        tchecker::clock_reset_t virt_tmp_A{i + no_of_orig_clocks_1 + no_of_orig_clocks_1 - 1, tchecker::REFCLOCK_ID, 0};
        tchecker::clock_reset_t virt_tmp_B{i + no_of_orig_clocks_1 + no_of_orig_clocks_2 - 1, tchecker::REFCLOCK_ID, 0};

        orig_reset_set_B.emplace_back(orig_tmp);

        virt_reset_set_A.emplace_back(virt_tmp_A);
        virt_reset_set_B.emplace_back(virt_tmp_B);
    }
  }

  tchecker::dbm::db_t dbm1_synced[dim1*dim1];
  tchecker::dbm::copy(dbm1_synced, dbm1, dim1);
  tchecker::dbm::db_t dbm2_synced[dim2*dim2];
  tchecker::dbm::copy(dbm2_synced, dbm2, dim2);

  sync(dbm1_synced, dbm2_synced, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2, orig_reset_set_A, orig_reset_set_B);

  assert(is_phi_subset_of_a_zone(dbm1_synced, dim1, no_of_orig_clocks_1, phi_e) && is_phi_subset_of_a_zone(dbm2_synced, dim2, no_of_orig_clocks_2, phi_e));
  assert(tchecker::dbm::status_t::EMPTY != tchecker::dbm::constrain(dbm1_synced, dim1, phi_e.get_vc(no_of_orig_clocks_1, true)));  
  assert(tchecker::dbm::status_t::EMPTY != tchecker::dbm::constrain(dbm2_synced, dim2, phi_e.get_vc(no_of_orig_clocks_2, true)));

  tchecker::dbm::db_t *multiple_reset = (tchecker::dbm::db_t *)malloc(dim1*dim1*sizeof(tchecker::dbm::db_t));

  enum tchecker::dbm::status_t stat_1 = revert_multiple_reset(multiple_reset, dbm1, dim1, dbm1_synced, virt_reset_set_A);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> first
    = tchecker::virtual_constraint::factory(multiple_reset, dim1, phi_e.get_no_of_virtual_clocks());

  free(multiple_reset);

  multiple_reset = (tchecker::dbm::db_t *)malloc(dim2*dim2*sizeof(tchecker::dbm::db_t));

  enum tchecker::dbm::status_t stat_2 = revert_multiple_reset(multiple_reset, dbm2, dim2, dbm2_synced, virt_reset_set_B);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> second
    = tchecker::virtual_constraint::factory(multiple_reset, dim2, phi_e.get_no_of_virtual_clocks());

  free(multiple_reset);

  orig_reset_set_A.clear();
  orig_reset_set_B.clear();


  virt_reset_set_A.clear();
  virt_reset_set_B.clear();

  if (tchecker::dbm::EMPTY == stat_1) {
    tchecker::dbm::empty(first->dbm(), first->dim());
  } else {
    assert(is_phi_subset_of_a_zone(dbm1, dim1, no_of_orig_clocks_1, *first));
  }

  if (tchecker::dbm::EMPTY == stat_2) {
    tchecker::dbm::empty(second->dbm(), second->dim());
  } else {
    assert(is_phi_subset_of_a_zone(dbm2, dim2, no_of_orig_clocks_2, *second));
  }

  return std::make_pair(first, second);
}

} // end of namespace vcg

} // end of namespace tchecker
