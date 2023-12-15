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

std::shared_ptr<vcg::virtual_constraint_t> zone_t::revert_action_trans(const tchecker::clock_constraint_container_t & guard,
                                                                       const tchecker::clock_reset_container_t & reset,
                                                                       const tchecker::clock_constraint_container_t & tgt_invariant,
                                                                       const vcg::virtual_constraint_t & phi_split)
{

  tchecker::clock_reset_container_t reset_copy;

  reset_copy.reserve(reset.size());

  std::copy(reset.begin(), reset.end(), reset_copy.begin());

  tchecker::dbm::db_t zone_clone[_dim*_dim];
  this->to_dbm(zone_clone);


  tchecker::dbm::db_t inter_zone[_dim*_dim];
  this->to_dbm(inter_zone);

  tchecker::dbm::constrain(inter_zone, _dim, guard);

  tchecker::dbm::db_t inter_clone[_dim*_dim];
  tchecker::dbm::copy(inter_clone, inter_zone, _dim);

  tchecker::dbm::reset(inter_zone, _dim, reset);
  tchecker::dbm::constrain(inter_zone, _dim, phi_split.get_vc());

  tchecker::vcg::virtual_constraint_t *virt_mult_reset 
      = vcg::factory(tchecker::dbm::revert_multiple_reset(inter_clone, _dim, inter_zone, reset_copy), _dim, phi_split.get_no_of_virt_clocks());

  tchecker::dbm::constrain(zone_clone, _dim, virt_mult_reset->get_vc());

  std::shared_ptr<tchecker::vcg::virtual_constraint_t> result = std::shared_ptr<vcg::virtual_constraint_t>(vcg::factory(zone_clone, _dim, phi_split.get_no_of_virt_clocks()));

  delete virt_mult_reset;

  return result;
}

std::shared_ptr<vcg::virtual_constraint_t> zone_t::revert_epsilon_trans(const tchecker::vcg::virtual_constraint_t & phi_split)
{

  tchecker::dbm::db_t *D_split = phi_split.to_dbm();
  tchecker::dbm::open_down(D_split, _dim);

  tchecker::dbm::intersection(D_split, this->dbm(), D_split, _dim);

  std::shared_ptr<vcg::virtual_constraint_t> result = 
    std::shared_ptr<tchecker::vcg::virtual_constraint_t>(vcg::factory(D_split, _dim, phi_split.get_no_of_virt_clocks()));

  free(D_split);

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

} // end of namespace zg

std::string to_string(tchecker::zg::zone_t const & zone, tchecker::clock_index_t const & index)
{
  std::stringstream sstream;
  zone.output(sstream, index);
  return sstream.str();
}

} // end of namespace tchecker
