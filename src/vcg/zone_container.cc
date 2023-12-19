/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/zone_container.hh"

namespace tchecker {

namespace vcg {

zone_container_t::zone_container_t(tchecker::clock_id_t dim) : _dim(dim) {}

bool zone_container_t::is_empty()
{
  return 0 == _storage.size();
}

void zone_container_t::append_zone()
{
  _storage.emplace_back(factory(_dim));
}

void zone_container_t::append_zone(zone_t const & zone)
{
  _storage.emplace_back(factory(_dim, zone));
}

void zone_container_t::append_zone(zone_t *zone)
{
  _storage.emplace_back(zone);
}

void zone_container_t::remove_back()
{
  tchecker::zg::zone_destruct_and_deallocate(*(_storage.begin()));
  _storage.erase(_storage.begin());
}

zone_t * zone_container_t::back()
{
  return _storage.back();
}

zone_t * zone_container_t::operator[](std::vector<zone_t>::size_type i)
{
  return _storage[i];
}

std::vector<zone_t>::size_type zone_container_t::size()
{
  return _storage.size();
}

std::vector<zone_t *>::iterator zone_container_t::begin()
{
  return _storage.begin();
}

std::vector<zone_t *>::iterator zone_container_t::end()
{
  return _storage.end();
}

zone_container_t::~zone_container_t()
{
  for(auto iter = _storage.begin(); iter < _storage.end(); ++iter) {
    tchecker::zg::zone_destruct_and_deallocate(*iter);
  }
}

} // end of namespace vcg

} // end of namespace tchecker
