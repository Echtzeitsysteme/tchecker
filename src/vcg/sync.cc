/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/sync.hh"
#include "tchecker/dbm/dbm.hh"


void sync(tchecker::dbm::db_t *dbm1, tchecker::dbm::db_t *dbm2, tchecker::clock_id_t dim, 
          tchecker::clock_id_t lowest_virt_clk_id, tchecker::clock_id_t no_of_orig_clocks_1,
          tchecker::clock_reset_container_t const & orig_reset1,
          tchecker::clock_reset_container_t const & orig_reset2)
{

  // TODO: add assertions

  tchecker::clock_reset_container_t virt_resets;

  for(const tchecker::clock_reset_t & r : orig_reset1) {

    if(r.right_id() != tchecker::REFCLOCK_ID || r.value() != 0) {
      throw std::runtime_error("when checking for timed bisim, only resets to value zero are allowed");
    }

    reset_to_value(dbm1, dim, r.left_id() + 1 + lowest_virt_clk_id, 0);
    reset_to_value(dbm2, dim, r.left_id() + 1 + lowest_virt_clk_id, 0);
  }

  tchecker::clock_id_t border = lowest_virt_clk_id + no_of_orig_clocks_1;

  for(const tchecker::clock_reset_t & r : orig_reset2) {

    if(r.right_id() != tchecker::REFCLOCK_ID || r.value() != 0) {
      throw std::runtime_error("when checking for timed bisim, only resets to value zero are allowed");
    }

    reset_to_value(dbm1, dim, r.left_id() + 1 + border, 0);
    reset_to_value(dbm2, dim, r.left_id() + 1 + border, 0);
  }

  virt_resets.clear();

}

std::tuple<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>>
revert_sync(const tchecker::dbm::db_t *dbm1, const tchecker::dbm::db_t *dbm2, tchecker::clock_id_t dim, 
            const tchecker::virtual_constraint::virtual_constraint_t & phi_e, tchecker::clock_id_t lowest_virt_clk_id,
            tchecker::clock_id_t no_of_orig_clocks_1)
{

  // TODO add assertions

  tchecker::dbm::db_t zero_value = tchecker::dbm::db(tchecker::LT, 0);
  tchecker::clock_reset_container_t reset_set;

  for(tchecker::clock_id_t i = 1; i < (dim - lowest_virt_clk_id); ++i) {
    if(zero_value == *tchecker::dbm::access(dbm1, dim, i, 0) && zero_value == *tchecker::dbm::access(dbm1, dim, 0, i)) {
        tchecker::clock_reset_t tmp{i + lowest_virt_clk_id - 1, tchecker::REFCLOCK_ID, 0};
        reset_set.emplace_back(tmp);
    }
  }

  for(tchecker::clock_id_t i = 1; i < (dim - lowest_virt_clk_id); ++i) {
    if(zero_value == *tchecker::dbm::access(dbm2, dim, i, 0) && zero_value == *tchecker::dbm::access(dbm2, dim, 0, i)) {
        tchecker::clock_reset_t tmp{i + lowest_virt_clk_id + no_of_orig_clocks_1 - 1, tchecker::REFCLOCK_ID, 0};
        reset_set.emplace_back(tmp);
    }
  }

  tchecker::dbm::db_t dbm1_clone[dim*dim];
  tchecker::dbm::copy(dbm1_clone, dbm1, dim);

  tchecker::dbm::constrain(dbm1_clone, dim, phi_e.get_vc(lowest_virt_clk_id - 1));

  tchecker::dbm::db_t dbm2_clone[dim*dim];
  tchecker::dbm::copy(dbm2_clone, dbm2, dim);

  tchecker::dbm::constrain(dbm2_clone, dim, phi_e.get_vc(lowest_virt_clk_id - 1));

  tchecker::dbm::db_t * multiple_reset = revert_multiple_reset(dbm1, dim, dbm1_clone, reset_set);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> first
    = tchecker::virtual_constraint::factory(multiple_reset, dim, dim - lowest_virt_clk_id);

  free(multiple_reset);

  multiple_reset = revert_multiple_reset(dbm2, dim, dbm2_clone, reset_set);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> second
    = tchecker::virtual_constraint::factory(multiple_reset, dim, dim - lowest_virt_clk_id);

  free(multiple_reset);

  reset_set.clear();

  return std::make_tuple(first, second);
}

