/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace virtual_constraint {

const tchecker::clock_id_t virtual_constraint_t::get_no_of_virt_clocks() const
{
  return this->dim() - 1;
}

// WARNING: above might not be tight after this call
void insert_values(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_change, tchecker::clock_id_t i, tchecker::clock_id_t j)
{

  tchecker::dbm::db_t * j_minus_i = tchecker::dbm::access(to_change->dbm(), to_change->dim(), j, i);
  tchecker::dbm::db_t * i_minus_j = tchecker::dbm::access(to_change->dbm(), to_change->dim(), i, j);

  tchecker::dbm::db_t db_above{
                        (tchecker::LE == tchecker::dbm::comparator(*i_minus_j)) ? tchecker::LT : tchecker::LE,
                        tchecker::dbm::value(*i_minus_j)};

  *j_minus_i = db_above;
  *i_minus_j = tchecker::dbm::LT_INFINITY;
}

// WARNING: The vitrual constraints placed at result are NOT tight!
void add_neg(tchecker::zone_container_t<virtual_constraint_t> *result, const virtual_constraint_t *cur, tchecker::clock_id_t i, tchecker::clock_id_t j)
{
    // we first insert all clock valuations for which chi_i - chi_j is to high
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> above = tchecker::virtual_constraint::factory(*cur);
  insert_values(above, i, j);
  result->append_zone(above);

  // now we insert all clocks for which chi_i - chi_j is to low
  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> below = tchecker::virtual_constraint::factory(*cur);
  insert_values(below, j, i);
  result->append_zone(below);
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> virtual_constraint_t::neg() const
{
  tchecker::zone_container_t<virtual_constraint_t> inter{this->dim()};

  for(tchecker::clock_id_t i = 0; i < this->dim(); ++i) {
    for(tchecker::clock_id_t j = i+1; j < this->dim(); ++j) {

      tchecker::clock_id_t size_before = inter.size();

      add_neg(&inter, this, i, j);

      // do the same for all already added virtual constraints.
      for(tchecker::clock_id_t iter = 0; iter < size_before; ++iter) {
        add_neg(&inter, &(*(inter[iter])), i, j);
      }
    }
  }

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(this->dim());

  for(auto iter = inter.begin(); iter < inter.end(); iter++) {
    if(tchecker::dbm::NON_EMPTY == tchecker::dbm::tighten((*iter)->dbm(), (*iter)->dim())) {
      result->append_zone(*iter);
    } else {
      tchecker::virtual_constraint::destruct(&(**iter));
    }
  }

  return result;
}

clock_constraint_container_t virtual_constraint_t::get_vc(tchecker::clock_id_t orig_clocks_offset) const
{
  clock_constraint_container_t result;

  for(tchecker::clock_id_t i = 1; i < get_no_of_virt_clocks(); ++i) {
    tchecker::clock_id_t cur = i + orig_clocks_offset;
    result.emplace_back(0, cur, tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), 0, i)->value);
    result.emplace_back(cur, 0, tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), i, 0)->value);

    for(tchecker::clock_id_t j = i+1; j < get_no_of_virt_clocks(); ++j) {
      tchecker::clock_id_t second_cur = j + orig_clocks_offset;
      result.emplace_back(second_cur, cur, tchecker::dbm::access(this->dbm(), this->dim(), j, i)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), j, i)->value);
      result.emplace_back(cur, second_cur, tchecker::dbm::access(this->dbm(), this->dim(), i, j)->cmp, tchecker::dbm::access(this->dbm(), this->dim(), i, j)->value);
    }
  }
  return result;

}

tchecker::zone_container_t<virtual_constraint_t> virtual_constraint_t::logic_and(tchecker::zone_container_t<virtual_constraint_t> *container)
{
  tchecker::zone_container_t<virtual_constraint_t> result{this->dim()};
  for(auto iter = container->begin(); iter < container->end(); iter++ ) {

    std::shared_ptr<virtual_constraint_t> to_append = factory(this->dim());
    if(tchecker::dbm::EMPTY != tchecker::dbm::intersection(to_append->dbm(), (*iter)->dbm(), this->dbm(), this->dim())) {
      result.append_zone(to_append);
    }
  }

  return result;
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::clock_id_t dim)
{
  return static_pointer_cast<tchecker::virtual_constraint::virtual_constraint_t>(
                                    tchecker::zg::factory(dim));
}

std::shared_ptr<virtual_constraint_t> factory(tchecker::virtual_constraint::virtual_constraint_t const & virtual_constraint)
{
  tchecker::zg::zone_t const & reinterpreted_zone = reinterpret_cast<tchecker::zg::zone_t const &>(virtual_constraint);
  return static_pointer_cast<tchecker::virtual_constraint::virtual_constraint_t>(
                                    tchecker::zg::factory(reinterpreted_zone)
        );
}


std::shared_ptr<virtual_constraint_t> factory(std::shared_ptr<tchecker::zg::zone_t const> zone, tchecker::clock_id_t no_of_virtual_clocks)
{
  return factory(zone->dbm(), zone->dim(), no_of_virtual_clocks);
}

std::shared_ptr<virtual_constraint_t> factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks)
{

  std::shared_ptr<virtual_constraint_t> result = factory(no_of_virtual_clocks + 1);

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
      tchecker::dbm::constrain(
          result->dbm(), result->dim(), i, j, dbm[indices[i]*dim + indices[j]].cmp, dbm[indices[i]*dim + indices[j]].value
        );
    }
  }

  return result;
}

void destruct(virtual_constraint_t *to_destruct)
{
  tchecker::zg::zone_destruct_and_deallocate(to_destruct);
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> combine_lhs_land_not_rhs(std::shared_ptr<virtual_constraint_t> lhs, std::shared_ptr<virtual_constraint_t> rhs, tchecker::clock_id_t dim)
{
  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(dim);

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> neg = rhs->neg();

  for(auto iter = neg->begin(); iter < neg->end(); iter++) {
    std::shared_ptr<virtual_constraint_t> to_append = factory(dim);
    if(tchecker::dbm::EMPTY != tchecker::dbm::intersection(to_append->dbm(), (*iter)->dbm(), lhs->dbm(), dim)) {
      result->append_zone(to_append);
    }
  }

  return result;
}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> combine_helper(std::shared_ptr<virtual_constraint_t> lhs, std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> lo_vc, tchecker::clock_id_t dim)
{
  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(dim);
  result->append_zone(*lhs);
  for(auto iter = lo_vc->begin(); iter < lo_vc->end(); iter++) { // cppcheck-suppress nullPointer
    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> inter = result;
    result = std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(dim);

    for(auto inter_iter = inter->begin(); inter_iter < inter->end(); inter_iter++) {
      std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> lhs_land_not_rhs = combine_lhs_land_not_rhs(*inter_iter, *iter, dim);
      for(auto land_iter = lhs_land_not_rhs->begin(); land_iter < lhs_land_not_rhs->end(); land_iter++) {
        result->append_zone(*land_iter);
      }
    }
  }

  return result;

}

std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> combine(std::vector<tchecker::zone_container_t<virtual_constraint_t>> & lo_lo_vc, tchecker::clock_id_t dim)
{
  if(lo_lo_vc.empty()) {
    return std::make_shared<tchecker::zone_container_t<virtual_constraint_t>>(dim);
  }

  tchecker::zone_container_t<virtual_constraint_t> cur = lo_lo_vc.back();
  lo_lo_vc.pop_back();

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = combine(lo_lo_vc, dim);

  for(auto cur_iter = cur.begin(); cur_iter < cur.end(); cur_iter++) {
    std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result = combine_helper(*cur_iter, result, dim);
  }
  return result;
}

} // end of namespace virtual_constraint

} // end of namespace tchecker
