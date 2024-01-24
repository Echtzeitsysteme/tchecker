/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/sync.hh"
#include "tchecker/dbm/dbm.hh"

namespace tchecker {

namespace vcg {

void sync(tchecker::dbm::db_t *dbm1, tchecker::dbm::db_t *dbm2,
          tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
          tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
          tchecker::clock_reset_container_t const & orig_reset1,
          tchecker::clock_reset_container_t const & orig_reset2)
{

  // TODO: add assertions

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

}

std::pair<std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>>
revert_sync(const tchecker::dbm::db_t *dbm1, const tchecker::dbm::db_t *dbm2,
            tchecker::clock_id_t dim1, tchecker::clock_id_t dim2,
            tchecker::clock_id_t no_of_orig_clocks_1, tchecker::clock_id_t no_of_orig_clocks_2,
            const tchecker::virtual_constraint::virtual_constraint_t & phi_e)
{

  // TODO add assertions
  // TODO there is some structural repetition here. A helping function might half the code size

  // due to the fact that the number of original clocks might differ, we need to build two reset sets

  tchecker::dbm::db_t zero_value = tchecker::dbm::db(tchecker::LT, 0);
  tchecker::clock_reset_container_t reset_set_A;
  tchecker::clock_reset_container_t reset_set_B;

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_1; ++i) {
    if(zero_value == *tchecker::dbm::access(dbm1, dim1, i, 0) && zero_value == *tchecker::dbm::access(dbm1, dim1, 0, i)) {
        tchecker::clock_reset_t tmp_A{i + no_of_orig_clocks_1, tchecker::REFCLOCK_ID, 0};
        tchecker::clock_reset_t tmp_B{i + no_of_orig_clocks_2, tchecker::REFCLOCK_ID, 0};

        reset_set_A.emplace_back(tmp_A);
        reset_set_B.emplace_back(tmp_B);
    }
  }

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_1; ++i) {
    if(zero_value == *tchecker::dbm::access(dbm2, dim2, i, 0) && zero_value == *tchecker::dbm::access(dbm2, dim2, 0, i)) {
        tchecker::clock_reset_t tmp_A{i + no_of_orig_clocks_1 + no_of_orig_clocks_1, tchecker::REFCLOCK_ID, 0};
        tchecker::clock_reset_t tmp_B{i + no_of_orig_clocks_1 + no_of_orig_clocks_2, tchecker::REFCLOCK_ID, 0};

        reset_set_A.emplace_back(tmp_A);
        reset_set_B.emplace_back(tmp_B);
    }
  }

  tchecker::dbm::db_t dbm1_clone[dim1*dim1];
  tchecker::dbm::copy(dbm1_clone, dbm1, dim1);

  tchecker::dbm::constrain(dbm1_clone, dim1, phi_e.get_vc(no_of_orig_clocks_1 + no_of_orig_clocks_2));

  tchecker::dbm::db_t dbm2_clone[dim2*dim2];
  tchecker::dbm::copy(dbm2_clone, dbm2, dim2);

  tchecker::dbm::constrain(dbm2_clone, dim2, phi_e.get_vc(no_of_orig_clocks_1 + no_of_orig_clocks_2));

  tchecker::dbm::db_t * multiple_reset = revert_multiple_reset(dbm1, dim1, dbm1_clone, reset_set_A);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> first
    = tchecker::virtual_constraint::factory(multiple_reset, dim1, no_of_orig_clocks_1 + no_of_orig_clocks_2);

  free(multiple_reset);

  multiple_reset = revert_multiple_reset(dbm2, dim2, dbm2_clone, reset_set_B);

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> second
    = tchecker::virtual_constraint::factory(multiple_reset, dim2, no_of_orig_clocks_1 + no_of_orig_clocks_2);

  free(multiple_reset);

  reset_set_A.clear();
  reset_set_B.clear();

  return std::make_pair(first, second);
}

} // end of namespace vcg

} // end of namespace tchecker
