/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <string>

#include "tchecker/dbm/dbm.hh"
#include "tchecker/zg/zone.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace zg {

tchecker::zg::zone_t & zone_t::operator=(tchecker::zg::zone_t const & zone)
{
  if (_dim != zone._dim)
    throw std::invalid_argument("Zone dimension mismatch");

  if (this != &zone)
    memcpy(dbm_ptr(), zone.dbm_ptr(), _dim * _dim * sizeof(tchecker::dbm::db_t));

  return *this;
}

bool zone_t::is_empty() const { return tchecker::dbm::is_empty_0(dbm_ptr(), _dim); }

bool zone_t::is_universal_positive() const { return tchecker::dbm::is_universal_positive(dbm_ptr(), _dim); }

void zone_t::make_universal() { tchecker::dbm::universal(dbm(), _dim); }

void zone_t::make_region(tchecker::clockval_t *valuation) {tchecker::dbm::reduce_to_valuation(dbm(), *valuation, _dim); }

bool zone_t::operator==(tchecker::zg::zone_t const & zone) const
{
  if (_dim != zone._dim)
    return false;
  bool empty1 = this->is_empty(), empty2 = zone.is_empty();
  if (empty1 || empty2)
    return (empty1 && empty2);
  return tchecker::dbm::is_equal(dbm_ptr(), zone.dbm_ptr(), _dim);
}

bool zone_t::operator!=(tchecker::zg::zone_t const & zone) const { return !(*this == zone); }

bool zone_t::operator<=(tchecker::zg::zone_t const & zone) const
{
  if (_dim != zone._dim)
    return false;
  if (this->is_empty())
    return true;
  if (zone.is_empty())
    return false;
  return tchecker::dbm::is_le(dbm_ptr(), zone.dbm_ptr(), _dim);
}

bool zone_t::is_am_le(tchecker::zg::zone_t const & zone, tchecker::clockbounds::map_t const & m) const
{
  if (this->is_empty())
    return true;
  if (zone.is_empty())
    return false;
  return tchecker::dbm::is_am_le(dbm_ptr(), zone.dbm_ptr(), _dim, m.ptr());
}

bool zone_t::is_alu_le(tchecker::zg::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                       tchecker::clockbounds::map_t const & u) const
{
  if (this->is_empty())
    return true;
  if (zone.is_empty())
    return false;
  return tchecker::dbm::is_alu_le(dbm_ptr(), zone.dbm_ptr(), _dim, l.ptr(), u.ptr());
}

int zone_t::lexical_cmp(tchecker::zg::zone_t const & zone) const
{
  return tchecker::dbm::lexical_cmp(dbm_ptr(), _dim, zone.dbm_ptr(), zone._dim);
}

std::size_t zone_t::hash() const { return tchecker::dbm::hash(dbm_ptr(), _dim); }

std::ostream & zone_t::output(std::ostream & os, tchecker::clock_index_t const & index) const
{
  return tchecker::dbm::output(os, dbm_ptr(), _dim,
                               [&](tchecker::clock_id_t id) { return (id == 0 ? "0" : index.value(id - 1)); });
}

tchecker::dbm::db_t * zone_t::dbm() { return dbm_ptr(); }

tchecker::dbm::db_t const * zone_t::dbm() const { return dbm_ptr(); }

void zone_t::to_dbm(tchecker::dbm::db_t * dbm) const { std::memcpy(dbm, dbm_ptr(), _dim * _dim * sizeof(*dbm)); }

bool zone_t::belongs(tchecker::clockval_t const & clockval) const
{
  return tchecker::dbm::satisfies(dbm_ptr(), _dim, clockval);
}

bool zone_t::is_virtual_equivalent(tchecker::zg::zone_t const & zone, tchecker::clock_id_t no_of_virt_clocks) const
{

  assert(no_of_virt_clocks < _dim);

  auto phi_this = tchecker::virtual_constraint::factory(*this, no_of_virt_clocks);
  auto phi_other = tchecker::virtual_constraint::factory(zone, no_of_virt_clocks);

  return (*phi_this) == (*phi_other);

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
zone_t::get_virtual_overhang(tchecker::virtual_constraint::virtual_constraint_t const & phi) const
{
  assert(phi.dim() <= this->dim());

  auto phi_this = tchecker::virtual_constraint::factory(*this, phi.get_no_of_virtual_clocks());

  auto result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(phi_this->dim());

  // if phi_this <= phi, then phi_this && not phi = emptyset
  if(*phi_this <= phi) {
    return result;
  }

  auto copy = factory(*this);

  // if this && phi = empty, then phi_this && not phi = phi_this
  if(tchecker::dbm::status_t::EMPTY == phi.logic_and(copy, *this)) {
    result->append_zone(phi_this);
    return result;
  }

  phi.neg_logic_and(result, *phi_this);
  result->compress();
  return result;
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
zone_t::get_virtual_overhang(tchecker::zg::zone_t const & other, tchecker::clock_id_t no_of_virtual_clocks) const
{
  auto phi_other = tchecker::virtual_constraint::factory(other, no_of_virtual_clocks);
  return this->get_virtual_overhang(*phi_other);
}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
zone_t::get_virtual_overhang_in_both_directions(tchecker::zg::zone_t const & other, tchecker::clock_id_t no_of_virtual_clocks) const
{
  auto result = this->get_virtual_overhang(other, no_of_virtual_clocks);
  result->append_container(other.get_virtual_overhang(*this, no_of_virtual_clocks));

  assert(
    std::all_of(result->begin(), result->end(),
                [no_of_virtual_clocks](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
                {
                  return (no_of_virtual_clocks == vc->get_no_of_virtual_clocks());
                }
    )
  );

  auto vc_A = tchecker::virtual_constraint::factory(*this, no_of_virtual_clocks);
  auto vc_B = tchecker::virtual_constraint::factory(other, no_of_virtual_clocks);
  assert(
    std::all_of(
      result->begin(),
      result->end(),
      [vc_A, vc_B](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> cur_con) {
        return *cur_con <= *vc_A || *cur_con <= *vc_B;
      }
    )
  );

  result->compress();
  return result;
}



zone_t::zone_t(tchecker::clock_id_t dim) : _dim(dim) { tchecker::dbm::universal_positive(dbm_ptr(), _dim); }

zone_t::zone_t(tchecker::zg::zone_t const & zone) : _dim(zone._dim)
{
  memcpy(dbm_ptr(), zone.dbm_ptr(), _dim * _dim * sizeof(tchecker::dbm::db_t));
}

zone_t::~zone_t() = default;

// Allocation and deallocation


void zone_destruct_and_deallocate(tchecker::zg::zone_t * zone)
{
  tchecker::zg::zone_t::destruct(zone);
  delete[] reinterpret_cast<char *>(zone);
}

std::shared_ptr<zone_t> factory(tchecker::clock_id_t dim)
{
  std::shared_ptr<zone_t> result(tchecker::zg::zone_allocate_and_construct(dim, dim), zone_destruct_and_deallocate);
  return result;
}

std::shared_ptr<zone_t> factory(zone_t const & zone)
{
  std::shared_ptr<zone_t> result(tchecker::zg::zone_allocate_and_construct(zone.dim(), zone), zone_destruct_and_deallocate);
  return result;
}

} // end of namespace zg

std::string to_string(tchecker::zg::zone_t const & zone, tchecker::clock_index_t const & index)
{
  std::stringstream sstream;
  zone.output(sstream, index);
  return sstream.str();
}

// specializations for the zone container
template<>
std::shared_ptr<tchecker::zg::zone_t> zone_container_t<tchecker::zg::zone_t>::create_element()
{
  return tchecker::zg::factory(_dim);
}

template<>
std::shared_ptr<tchecker::zg::zone_t> zone_container_t<tchecker::zg::zone_t>::create_element(tchecker::zg::zone_t const & zone)
{
  return tchecker::zg::factory(zone);
}


} // end of namespace tchecker
