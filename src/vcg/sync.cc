/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/sync.hh"

namespace tchecker {

namespace vcg {

bool is_virtually_equivalent(tchecker::dbm::db_t *dbm1, tchecker::dbm::db_t *dbm2,
                                tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
                                tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2)
{
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc1 
    = tchecker::virtual_constraint::factory(dbm1, dim1, dim1 - no_of_orig_clocks_1 - 1);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc2 
    = tchecker::virtual_constraint::factory(dbm2, dim2, dim2 - no_of_orig_clocks_2 - 1);

  return (*vc1 == *vc2);
}


bool are_dbm_synced(tchecker::dbm::db_t *dbm1, tchecker::dbm::db_t *dbm2,
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

  assert(dim1 == no_of_orig_clocks_1 + no_of_orig_clocks_1 + no_of_orig_clocks_2 + 1);
  assert(dim2 == no_of_orig_clocks_1 + no_of_orig_clocks_2 + no_of_orig_clocks_2 + 1);

  bool result = is_virtually_equivalent(dbm1, dbm2, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2);

  if(!result) {
    std::cout << __FILE__ << ": " << __LINE__ << ": the dbm are not virtually equivalent." << std::endl;
    tchecker::dbm::output_matrix(std::cout, dbm1, dim1);
    tchecker::dbm::output_matrix(std::cout, dbm2, dim2);
    return result;
  }

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_1; ++i) {
    tchecker::dbm::db_t * orig_min_virt = tchecker::dbm::access(dbm1, dim1, i, i + no_of_orig_clocks_1);
    tchecker::dbm::db_t * virt_min_orig = tchecker::dbm::access(dbm1, dim1, i + no_of_orig_clocks_1, i);

    result &= (*orig_min_virt == *virt_min_orig);
    result &= (orig_min_virt->cmp == tchecker::LE);
    result &= (orig_min_virt->value == 0);
    if(!result) {
      std::cout << __FILE__ << ": " << __LINE__ << ": " << i << " virtual and original clock of first dbm are not equivalent" << std::endl;
      tchecker::dbm::output_matrix(std::cout, dbm1, dim1);
      return result;
    }
  }

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_2; ++i) {
    tchecker::dbm::db_t * orig_min_virt = tchecker::dbm::access(dbm2, dim2, i, i + no_of_orig_clocks_1 + no_of_orig_clocks_2);
    tchecker::dbm::db_t * virt_min_orig = tchecker::dbm::access(dbm2, dim2, i + no_of_orig_clocks_1 + no_of_orig_clocks_2, i);

    result &= (*orig_min_virt == *virt_min_orig);
    result &= (orig_min_virt->cmp == tchecker::LE);
    result &=(orig_min_virt->value == 0);
    if(!result) {
      std::cout << __FILE__ << ": " << __LINE__ << ": " << i << " virtual and original clock of second dbm are not equivalent" << std::endl;
      tchecker::dbm::output_matrix(std::cout, dbm2, dim2);
      return result;
    }
  }

  return result;
}

bool is_phi_subset_of_a_zone(const tchecker::dbm::db_t *dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_orig_clocks,
                             const tchecker::virtual_constraint::virtual_constraint_t & phi_e)
{
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> phi = tchecker::virtual_constraint::factory(dbm, dim, dim - no_of_orig_clocks - 1);

  return tchecker::dbm::is_le(phi_e.dbm(), phi->dbm(), phi_e.dim());
}

void sync(tchecker::dbm::db_t *dbm1, tchecker::dbm::db_t *dbm2,
          tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
          tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
          tchecker::clock_reset_container_t const & orig_reset1,
          tchecker::clock_reset_container_t const & orig_reset2)
{

  assert(is_virtually_equivalent(dbm1, dbm2, dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2));

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



std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>>
revert_sync(const tchecker::dbm::db_t *dbm1, const tchecker::dbm::db_t *dbm2,
            tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
            tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
            const tchecker::virtual_constraint::virtual_constraint_t & phi_e)
{

  assert(is_virtually_equivalent(const_cast<tchecker::dbm::db_t *>(dbm1), const_cast<tchecker::dbm::db_t *>(dbm2), dim1, dim2, no_of_orig_clocks_1, no_of_orig_clocks_2));

  // due to the fact that the number of original clocks might differ, we need to build two reset sets
  tchecker::clock_reset_container_t orig_reset_set_A;
  tchecker::clock_reset_container_t orig_reset_set_B;

  tchecker::clock_reset_container_t virt_reset_set_A;
  tchecker::clock_reset_container_t virt_reset_set_B;

  for(tchecker::clock_id_t i = 0; i < no_of_orig_clocks_1; ++i) {
    if(tchecker::dbm::LE_ZERO == *tchecker::dbm::access(dbm1, dim1, i+1, 0) && tchecker::dbm::LE_ZERO == *tchecker::dbm::access(dbm1, dim1, 0, i+1)) {

        tchecker::clock_reset_t orig_tmp{i, tchecker::REFCLOCK_ID, 0};

        tchecker::clock_reset_t virt_tmp_A{i + no_of_orig_clocks_1, tchecker::REFCLOCK_ID, 0};
        tchecker::clock_reset_t virt_tmp_B{i + no_of_orig_clocks_2, tchecker::REFCLOCK_ID, 0};

        orig_reset_set_A.emplace_back(orig_tmp);

        virt_reset_set_A.emplace_back(virt_tmp_A);
        virt_reset_set_B.emplace_back(virt_tmp_B);
    }
  }

  for(tchecker::clock_id_t i = 0; i < no_of_orig_clocks_2; ++i) {
    if(tchecker::dbm::LE_ZERO == *tchecker::dbm::access(dbm2, dim2, i+1, 0) && tchecker::dbm::LE_ZERO == *tchecker::dbm::access(dbm2, dim2, 0, i+1)) {

        tchecker::clock_reset_t orig_tmp{i, tchecker::REFCLOCK_ID, 0};

        tchecker::clock_reset_t virt_tmp_A{i + no_of_orig_clocks_1 + no_of_orig_clocks_1, tchecker::REFCLOCK_ID, 0};
        tchecker::clock_reset_t virt_tmp_B{i + no_of_orig_clocks_1 + no_of_orig_clocks_2, tchecker::REFCLOCK_ID, 0};

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

  assert(is_phi_subset_of_a_zone(dbm1_synced, dim1, no_of_orig_clocks_1, phi_e) || is_phi_subset_of_a_zone(dbm2_synced, dim2, no_of_orig_clocks_2, phi_e));

  if(tchecker::dbm::status_t::EMPTY == tchecker::dbm::constrain(dbm1_synced, dim1, phi_e.get_vc(no_of_orig_clocks_1, true))) {
    throw std::runtime_error("problems in _A while reverting the sync"); // should NEVER occur
  }

  if(tchecker::dbm::status_t::EMPTY == tchecker::dbm::constrain(dbm2_synced, dim2, phi_e.get_vc(no_of_orig_clocks_2, true))) {
    throw std::runtime_error("problems in _B while reverting the sync"); // should NEVER occur
  }

  tchecker::dbm::db_t * multiple_reset = revert_multiple_reset(dbm1, dim1, dbm1_synced, virt_reset_set_A);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> first
    = tchecker::virtual_constraint::factory(multiple_reset, dim1, no_of_orig_clocks_1 + no_of_orig_clocks_2);

  free(multiple_reset);

  multiple_reset = revert_multiple_reset(dbm2, dim2, dbm2_synced, virt_reset_set_B);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> second
    = tchecker::virtual_constraint::factory(multiple_reset, dim2, no_of_orig_clocks_1 + no_of_orig_clocks_2);

  free(multiple_reset);

  orig_reset_set_A.clear();
  orig_reset_set_B.clear();


  virt_reset_set_A.clear();
  virt_reset_set_B.clear();

  assert(is_phi_subset_of_a_zone(dbm1, dim1, no_of_orig_clocks_1, *first));
  assert(is_phi_subset_of_a_zone(dbm2, dim2, no_of_orig_clocks_2, *second));

  return std::make_pair(first, second);
}

} // end of namespace vcg

} // end of namespace tchecker
