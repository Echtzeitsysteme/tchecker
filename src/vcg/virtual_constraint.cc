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

tchecker::clock_id_t virtual_constraint_t::get_no_of_virtual_clocks() const
{
  return this->dim() - 1;
}

// WARNING: The vitrual constraints placed at result are NOT tight!
void add_neg_single(tchecker::zone_container_t<virtual_constraint_t> *result, const virtual_constraint_t & cur, tchecker::clock_id_t i, tchecker::clock_id_t j, tchecker::dbm::db_t *max_value)
{

  // the first line is a special case since the value of 0 - c cannot become larger than zero.
  tchecker::dbm::db_t upper_bound = ((0 == i) && (tchecker::dbm::LE_ZERO < *max_value)) ? tchecker::dbm::LE_ZERO : *max_value;

  // if the upper constraint is larger than the upper bound, the inverse is empty anyway.
  if(*tchecker::dbm::access(cur.dbm(), cur.dim(), i, j) < upper_bound) {

    tchecker::dbm::db_t copy{tchecker::dbm::invert(*tchecker::dbm::access(cur.dbm(), cur.dim(), i, j))};
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_insert = tchecker::virtual_constraint::factory(cur);

    // we remove what is allowed by cur, by setting the entry j - i to the inverse of cur[i][j], and add what is not allowed, by setting i - j to LT_INFINITY

    tchecker::dbm::db_t * j_minus_i = tchecker::dbm::access(to_insert->dbm(), to_insert->dim(), j, i);
    tchecker::dbm::db_t * i_minus_j = tchecker::dbm::access(to_insert->dbm(), to_insert->dim(), i, j);

    *j_minus_i = copy;
    *i_minus_j = upper_bound;

    result->append_zone(to_insert);
  }
}

void add_neg(tchecker::zone_container_t<virtual_constraint_t> *result, const virtual_constraint_t & cur, 
        tchecker::clock_id_t i, tchecker::clock_id_t j, 
        tchecker::dbm::db_t *max_i_minus_j, tchecker::dbm::db_t *max_j_minus_i)
{

  // we first add the neg breaking the upper bound of i-j
  add_neg_single(result, cur, i, j, max_i_minus_j);
  // we afterwards add the neg breaking the upper bound of j - i
  add_neg_single(result, cur, j, i, max_j_minus_i);
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> virtual_constraint_t::neg_helper(tchecker::dbm::db_t *upper_bounds) const
{
  tchecker::zone_container_t<virtual_constraint_t> inter{this->dim()};

  for(tchecker::clock_id_t i = 0; i < this->dim(); ++i) {
    for(tchecker::clock_id_t j = i+1; j < this->dim(); ++j) {

      tchecker::zone_container_t<virtual_constraint_t> helper{this->dim()};
      helper.append_container(inter); // this way, only the pointers are copied.

      tchecker::dbm::db_t * i_minus_j = tchecker::dbm::access(upper_bounds, this->dim(), i, j);
      tchecker::dbm::db_t * j_minus_i = tchecker::dbm::access(upper_bounds, this->dim(), j, i);

      add_neg(&inter, *this, i, j, i_minus_j, j_minus_i);

      // do the same for all already added virtual constraints.
      for(auto iter = helper.begin(); iter < helper.end(); iter++) {
        add_neg(&inter, **iter, i, j, i_minus_j, j_minus_i);
      }
    }
  }

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(this->dim());

  for(auto iter = inter.begin(); iter < inter.end(); iter++) {
    if(tchecker::dbm::NON_EMPTY ==tchecker::dbm::tighten((*iter)->dbm(), (*iter)->dim()) &&
       tchecker::dbm::NON_EMPTY == tchecker::dbm::intersection((*iter)->dbm(), (*iter)->dbm(), upper_bounds, (*iter)->dim())) {
      result->append_zone(*iter);
    }
  }

  result->compress();

  return result;
}

clock_constraint_container_t virtual_constraint_t::get_vc(tchecker::clock_id_t no_of_orig_clocks, bool system_clocks) const
{

  tchecker::clock_id_t ref_clk = system_clocks ? tchecker::REFCLOCK_ID : 0;
  clock_constraint_container_t result;

  for(tchecker::clock_id_t i = 1; i <= get_no_of_virtual_clocks(); ++i) {
    tchecker::clock_id_t cur = i + no_of_orig_clocks;

    if(system_clocks) {
      cur--;
    }

    result.emplace_back(ref_clk, cur, tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->value);
    result.emplace_back(cur, ref_clk, tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->value);

    for(tchecker::clock_id_t j = i+1; j <= get_no_of_virtual_clocks(); ++j) {
      tchecker::clock_id_t second_cur = j + no_of_orig_clocks;

    if(system_clocks) {
      second_cur--;
    }

      result.emplace_back(second_cur, cur, tchecker::dbm::access(this->dbm(), this->dim(), j, i)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), j, i)->value);
      result.emplace_back(cur, second_cur, tchecker::dbm::access(this->dbm(), this->dim(), i, j)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), i, j)->value);
    }
  }

  return result;

}

void virtual_constraint_t::neg_logic_and(std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result, const virtual_constraint_t & other) const {
  assert(result->is_empty());
  assert(result->dim() == other.dim());
  assert(this->dim() == other.dim());

  result->append_container(this->neg_helper(const_cast<tchecker::dbm::db_t *>(other.dbm())));
}

enum tchecker::dbm::status_t virtual_constraint_t::logic_and(std::shared_ptr<virtual_constraint_t> result, const virtual_constraint_t & other) const
{
  assert(result->dim() == other.dim());
  assert(this->dim() == result->dim());
  return tchecker::dbm::intersection(result->dbm(), other.dbm(), this->dbm(), this->dim());
}

enum tchecker::dbm::status_t virtual_constraint_t::logic_and(std::shared_ptr<tchecker::zg::zone_t> result, const tchecker::zg::zone_t & zone) const
{
  return this->logic_and(*result, zone);
}

enum tchecker::dbm::status_t virtual_constraint_t::logic_and(tchecker::zg::zone_t & result, const tchecker::zg::zone_t & zone) const
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

  for(auto iter = container->begin(); iter < container->end(); iter++ ) {
    std::shared_ptr<virtual_constraint_t> to_append = factory(this->get_no_of_virtual_clocks());
    if(tchecker::dbm::EMPTY != tchecker::dbm::intersection(to_append->dbm(), (*iter)->dbm(), this->dbm(), this->dim())) {
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

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_first; ++i) {
    tchecker::dbm::reset_to_clock(first->dbm(), dim_first, i, i+no_of_orig_clocks_first);
  }

  tchecker::clock_id_t dim_second = no_of_orig_clocks_second + this->dim();
  auto second = tchecker::zg::factory(dim_second);
  second->make_universal();
  this->logic_and(second, *second);

  for(tchecker::clock_id_t i = 1; i <= no_of_orig_clocks_second; ++i) {
    tchecker::dbm::reset_to_clock(second->dbm(), dim_second, i, i+no_of_orig_clocks_second + no_of_orig_clocks_first);
  }

  return std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>(first, second);
}                                                                                    

std::shared_ptr<virtual_constraint_t> factory(tchecker::clock_id_t number_of_virtual_clocks)
{
  return static_pointer_cast<tchecker::virtual_constraint::virtual_constraint_t>(
                                    tchecker::zg::factory(number_of_virtual_clocks + 1));
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::virtual_constraint::virtual_constraint_t const & virtual_constraint)
{
  return factory(virtual_constraint.dbm(), virtual_constraint.dim(), virtual_constraint.get_no_of_virtual_clocks());
}


std::shared_ptr<virtual_constraint_t> factory(std::shared_ptr<tchecker::zg::zone_t const> zone, tchecker::clock_id_t no_of_virtual_clocks)
{
  return factory(zone->dbm(), zone->dim(), no_of_virtual_clocks);
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::zg::zone_t const & zone, tchecker::clock_id_t no_of_virtual_clocks)
{
  return factory(zone.dbm(), zone.dim(), no_of_virtual_clocks);
}

std::shared_ptr<virtual_constraint_t> factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks)
{

  assert(dim > no_of_virtual_clocks);

  std::shared_ptr<virtual_constraint_t> result = factory(no_of_virtual_clocks);

  std::vector<tchecker::clock_id_t> indices;
  indices.emplace_back(0);

  for(tchecker::clock_id_t  i = dim - no_of_virtual_clocks; i < dim; ++i)
  {
    indices.emplace_back(i);
  }

  for(tchecker::clock_id_t i = 0; i < indices.size(); ++i)
  {
    for(tchecker::clock_id_t j = 0; j < indices.size(); ++j)
    {
      tchecker::dbm::db_t * i_minus_j = tchecker::dbm::access(result->dbm(), result->dim(), i, j);
      *i_minus_j = *(tchecker::dbm::access(dbm, dim, indices[i], indices[j]));
    }
  }

  return result;
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> combine(tchecker::zone_container_t<virtual_constraint_t> & lo_vc, tchecker::clock_id_t no_of_virtual_clocks)
{

  lo_vc.compress();

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(no_of_virtual_clocks + 1);

  for(auto iter = lo_vc.begin(); iter < lo_vc.end(); iter++) {

    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> inter = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(no_of_virtual_clocks + 1);
    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> helper;
    inter->append_zone(*iter);

    for(auto phi_r = result->begin(); phi_r < result->end(); phi_r++) {
      helper = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(no_of_virtual_clocks + 1);
      for(auto phi_inter = inter->begin(); phi_inter < inter->end(); phi_inter++) {
        std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> to_append
          = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(no_of_virtual_clocks + 1);

        (*phi_r)->neg_logic_and(to_append, **phi_inter);

        helper->append_container(to_append);
      }
      inter = helper;
    }

    for(auto phi = inter->begin(); phi < inter->end(); phi++) {
      if(!(*phi)->is_empty()) {
        assert(tchecker::dbm::is_le((*phi)->dbm(), (*iter)->dbm(), (*iter)->dim()));
        result->append_zone(**phi);
      }
    }
    result->compress();
  }

  return result;
}

bool all_elements_are_stronger_than(std::shared_ptr<zone_container_t<virtual_constraint_t>> weaker, 
                                    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> stronger, tchecker::clock_id_t no_of_virtual_clocks) {
  bool result = true;
  for(auto iter = stronger->begin(); iter < stronger->end(); iter++) {
    result &= ((*iter)->dim() == (no_of_virtual_clocks + 1));
    bool inter = false;
    for(auto second_iter = weaker->begin(); second_iter < weaker->end(); second_iter++) {
      inter |= tchecker::dbm::is_le((*iter)->dbm(), (*second_iter)->dbm(), no_of_virtual_clocks + 1);
    }
    result &= inter;
  }
  return result;
}

} // end of namespace virtual_constraint

// specializations for the zone container
template<>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element()
{
  return tchecker::virtual_constraint::factory(_dim - 1);
}

template<>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element(tchecker::virtual_constraint::virtual_constraint_t const & zone)
{
  return tchecker::virtual_constraint::factory(zone);
}


} // end of namespace tchecker
