/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/vcg/revert_transitions.hh"

namespace tchecker {

namespace virtual_constraint {

tchecker::clock_id_t virtual_constraint_t::get_no_of_virtual_clocks() const { return this->dim() - 1; }

static bool constrain_db(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
                         tchecker::dbm::db_t db)
{
  tchecker::dbm::db_t * entry = tchecker::dbm::access(dbm, dim, x, y);
  if (!(db < *entry)) {
    return true;
  }
  *entry = db;
  return tchecker::dbm::tighten(dbm, dim, x, y) != tchecker::dbm::EMPTY;
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>>
virtual_constraint_t::neg_helper(tchecker::dbm::db_t * upper_bounds) const
{
  tchecker::clock_id_t const dim = this->dim();
  auto result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(dim);

  // prefix = upper_bounds && (all clocks >= 0) && c_1 && ... && c_{m-1}
  std::shared_ptr<virtual_constraint_t> prefix = tchecker::virtual_constraint::factory(dim - 1);
  tchecker::dbm::copy(prefix->dbm(), upper_bounds, dim);
  for (tchecker::clock_id_t j = 1; j < dim; ++j) {
    tchecker::dbm::db_t * zero_minus_j = tchecker::dbm::access(prefix->dbm(), dim, 0, j);
    if (tchecker::dbm::LE_ZERO < *zero_minus_j) {
      *zero_minus_j = tchecker::dbm::LE_ZERO; // clock values are non-negative
    }
  }

  if (tchecker::dbm::NON_EMPTY == tchecker::dbm::tighten(prefix->dbm(), dim)) {
    bool prefix_empty = false;
    for (tchecker::clock_id_t i = 0; i < dim && !prefix_empty; ++i) {
      for (tchecker::clock_id_t j = 0; j < dim && !prefix_empty; ++j) {
        if (i == j)
          continue;
        tchecker::dbm::db_t const c = *tchecker::dbm::access(this->dbm(), dim, i, j);
        if (!(c < *tchecker::dbm::access(prefix->dbm(), dim, i, j)))
          continue; // c is implied by prefix, so prefix && !c is empty
        // piece = prefix && !(x_i - x_j # c) = prefix && (x_j - x_i #' -c)
        std::shared_ptr<virtual_constraint_t> piece = tchecker::virtual_constraint::factory(*prefix);
        if (constrain_db(piece->dbm(), dim, j, i, tchecker::dbm::invert(c)))
          result->append_zone(piece);
        prefix_empty = !constrain_db(prefix->dbm(), dim, i, j, c);
      }
    }
  }

  result->compress();

  return result;
}

clock_constraint_container_t virtual_constraint_t::get_vc(tchecker::clock_id_t no_of_orig_clocks, bool system_clocks) const
{

  tchecker::clock_id_t ref_clk = system_clocks ? tchecker::REFCLOCK_ID : 0;
  clock_constraint_container_t result;

  for (tchecker::clock_id_t i = 1; i <= get_no_of_virtual_clocks(); ++i) {
    tchecker::clock_id_t cur = i + no_of_orig_clocks - system_clocks;

    result.emplace_back(ref_clk, cur, tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->cmp,
                        tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->value);
    result.emplace_back(cur, ref_clk, tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->cmp,
                        tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->value);

    for (tchecker::clock_id_t j = i + 1; j <= get_no_of_virtual_clocks(); ++j) {
      tchecker::clock_id_t second_cur = j + no_of_orig_clocks;

      if (system_clocks) {
        second_cur--;
      }

      result.emplace_back(second_cur, cur, tchecker::dbm::access(this->dbm(), this->dim(), j, i)->cmp,
                          tchecker::dbm::access(this->dbm(), this->dim(), j, i)->value);
      result.emplace_back(cur, second_cur, tchecker::dbm::access(this->dbm(), this->dim(), i, j)->cmp,
                          tchecker::dbm::access(this->dbm(), this->dim(), i, j)->value);
    }
  }

  return result;
}

clock_constraint_container_t virtual_constraint_t::convert_to_original_constraint(bool system_clocks) const
{

  tchecker::clock_id_t ref_clk = system_clocks ? tchecker::REFCLOCK_ID : 0;
  clock_constraint_container_t result;

  for (tchecker::clock_id_t i = 1; i <= get_no_of_virtual_clocks(); ++i) {
    tchecker::clock_id_t cur = i - system_clocks;

    result.emplace_back(ref_clk, cur, tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->cmp,
                        tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->value);
    result.emplace_back(cur, ref_clk, tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->cmp,
                        tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->value);

    for (tchecker::clock_id_t j = i + 1; j <= get_no_of_virtual_clocks(); ++j) {
      tchecker::clock_id_t second_cur = j - system_clocks;

      result.emplace_back(second_cur, cur, tchecker::dbm::access(this->dbm(), this->dim(), j, i)->cmp,
                          tchecker::dbm::access(this->dbm(), this->dim(), j, i)->value);
      result.emplace_back(cur, second_cur, tchecker::dbm::access(this->dbm(), this->dim(), i, j)->cmp,
                          tchecker::dbm::access(this->dbm(), this->dim(), i, j)->value);
    }
  }

  return result;
}

void virtual_constraint_t::neg_logic_and(std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result,
                                         const virtual_constraint_t & other) const
{
  assert(result->is_empty());
  assert(result->dim() == other.dim());
  assert(this->dim() == other.dim());

  result->append_container(this->neg_helper(const_cast<tchecker::dbm::db_t *>(other.dbm())));
}

enum tchecker::dbm::status_t virtual_constraint_t::logic_and(std::shared_ptr<virtual_constraint_t> result,
                                                             const virtual_constraint_t & other) const
{
  assert(result->dim() == other.dim());
  assert(this->dim() == result->dim());
  return tchecker::dbm::intersection(result->dbm(), other.dbm(), this->dbm(), this->dim());
}

enum tchecker::dbm::status_t virtual_constraint_t::logic_and(std::shared_ptr<tchecker::zg::zone_t> result,
                                                             const tchecker::zg::zone_t & zone) const
{
  return this->logic_and(*result, zone);
}

enum tchecker::dbm::status_t virtual_constraint_t::logic_and(tchecker::zg::zone_t & result,
                                                             const tchecker::zg::zone_t & zone) const
{
  assert(result.dim() == zone.dim());
  assert(tchecker::dbm::is_tight(zone.dbm(), zone.dim()));
  tchecker::dbm::copy(result.dbm(), zone.dbm(), zone.dim());
  return tchecker::dbm::constrain(result.dbm(), result.dim(), this->get_vc(result.dim() - this->dim(), true));
}

void virtual_constraint_t::logic_and(std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result,
                                     std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> const container) const
{
  assert(result->is_empty());
  assert(result->dim() == container->dim());
  assert(this->dim() == container->dim());

  for (auto iter = container->begin(); iter < container->end(); iter++) {
    std::shared_ptr<virtual_constraint_t> to_append = factory(this->get_no_of_virtual_clocks());
    if (tchecker::dbm::EMPTY != tchecker::dbm::intersection(to_append->dbm(), (*iter)->dbm(), this->dbm(), this->dim())) {
      result->append_zone(to_append);
    }
  }
}

std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>
virtual_constraint_t::generate_synchronized_zones(tchecker::clock_id_t no_of_orig_clocks_first,
                                                  tchecker::clock_id_t no_of_orig_clocks_second)
{
  tchecker::clock_id_t dim_first = no_of_orig_clocks_first + this->dim();
  auto first = tchecker::zg::factory(dim_first);
  first->make_universal();
  this->logic_and(first, *first);

  for (tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_first; ++i) {
    tchecker::dbm::reset_to_clock(first->dbm(), dim_first, i, i + no_of_orig_clocks_first);
  }

  tchecker::clock_id_t dim_second = no_of_orig_clocks_second + this->dim();
  auto second = tchecker::zg::factory(dim_second);
  second->make_universal();
  this->logic_and(second, *second);

  for (tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_second; ++i) {
    tchecker::dbm::reset_to_clock(second->dbm(), dim_second, i, i + no_of_orig_clocks_second + no_of_orig_clocks_first);
  }

  return std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>(first, second);
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::clock_id_t number_of_virtual_clocks)
{
  return static_pointer_cast<tchecker::virtual_constraint::virtual_constraint_t>(
      tchecker::zg::factory(number_of_virtual_clocks + 1));
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::virtual_constraint::virtual_constraint_t const & virtual_constraint, bool ignore_urgent_clk)
{
  assert(virtual_constraint.dim() == virtual_constraint.get_no_of_virtual_clocks() + 1);
  return factory(virtual_constraint.dbm(), virtual_constraint.dim(), virtual_constraint.get_no_of_virtual_clocks(), ignore_urgent_clk);
}

std::shared_ptr<virtual_constraint_t> factory(std::shared_ptr<tchecker::zg::zone_t const> zone,
                                              tchecker::clock_id_t no_of_virtual_clocks, bool ignore_urgent_clk)
{
  return factory(zone->dbm(), zone->dim(), no_of_virtual_clocks, ignore_urgent_clk);
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::zg::zone_t const & zone, tchecker::clock_id_t no_of_virtual_clocks, bool ignore_urgent_clk)
{
  return factory(zone.dbm(), zone.dim(), no_of_virtual_clocks, ignore_urgent_clk);
}

std::shared_ptr<virtual_constraint_t> factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                              tchecker::clock_id_t no_of_virtual_clocks, bool ignore_urgent_clk)
{

  assert(dim > no_of_virtual_clocks);

  tchecker::clock_id_t used_no_virt_clks = ignore_urgent_clk ? no_of_virtual_clocks - 1 : no_of_virtual_clocks;

  std::shared_ptr<virtual_constraint_t> result = factory(used_no_virt_clks);

  std::vector<tchecker::clock_id_t> indices;
  indices.emplace_back(0);

  for (tchecker::clock_id_t i = dim - no_of_virtual_clocks; i < dim - (ignore_urgent_clk ? 1 : 0); ++i) {
    indices.emplace_back(i);
  }

  for (tchecker::clock_id_t i = 0; i < indices.size(); ++i) {
    for (tchecker::clock_id_t j = 0; j < indices.size(); ++j) {
      tchecker::dbm::db_t * i_minus_j = tchecker::dbm::access(result->dbm(), result->dim(), i, j);
      *i_minus_j = *(tchecker::dbm::access(dbm, dim, indices[i], indices[j]));
    }
  }

  return result;
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>>
combine(tchecker::zone_container_t<virtual_constraint_t> & lo_vc, tchecker::clock_id_t no_of_virtual_clocks)
{

  lo_vc.compress();

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result =
      std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(no_of_virtual_clocks + 1);

  for (auto iter = lo_vc.begin(); iter < lo_vc.end(); iter++) {

    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> inter =
        std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(no_of_virtual_clocks + 1);
    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> helper;
    inter->append_zone(*iter);

    for (auto phi_r = result->begin(); phi_r < result->end(); phi_r++) {
      helper = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(no_of_virtual_clocks + 1);
      for (auto phi_inter = inter->begin(); phi_inter < inter->end(); phi_inter++) {
        std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_append =
            std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(
                no_of_virtual_clocks + 1);

        (*phi_r)->neg_logic_and(to_append, **phi_inter);

        helper->append_container(to_append);
      }
      inter = helper;
    }

    for (auto phi = inter->begin(); phi < inter->end(); phi++) {
      if (!(*phi)->is_empty()) {
        assert(tchecker::dbm::is_le((*phi)->dbm(), (*iter)->dbm(), (*iter)->dim()));
        result->append_zone(**phi);
      }
    }
    result->compress();
  }

  return result;
}

bool all_elements_are_stronger_than(std::shared_ptr<zone_container_t<virtual_constraint_t>> weaker,
                                    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> stronger,
                                    tchecker::clock_id_t no_of_virtual_clocks)
{
  bool result = true;
  for (auto iter = stronger->begin(); iter < stronger->end(); iter++) {
    result &= ((*iter)->dim() == (no_of_virtual_clocks + 1));
    bool inter = false;
    for (auto second_iter = weaker->begin(); second_iter < weaker->end(); second_iter++) {
      inter |= tchecker::dbm::is_le((*iter)->dbm(), (*second_iter)->dbm(), no_of_virtual_clocks + 1);
    }
    result &= inter;
  }
  return result;
}

} // end of namespace virtual_constraint

// specializations for the zone container
template <>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element()
{
  return tchecker::virtual_constraint::factory(_dim - 1);
}

template <>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element(
    tchecker::virtual_constraint::virtual_constraint_t const & zone)
{
  return tchecker::virtual_constraint::factory(zone);
}

} // end of namespace tchecker
