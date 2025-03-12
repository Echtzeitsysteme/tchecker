/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/visited_map.hh"

namespace tchecker {

namespace strong_timed_bisim {

visited_map_t::visited_map_t(tchecker::clock_id_t no_of_virtual_clocks) :  _no_of_virtual_clocks(no_of_virtual_clocks), _storage(std::make_shared<visited_map_storage_t>()) {}

visited_map_t::visited_map_t(visited_map_t &t) : _no_of_virtual_clocks(t.no_of_virtual_clocks()), _storage(std::make_shared<visited_map_storage_t>()) 
{   
    for(auto iter = t.begin(); iter != t.end(); iter++) {
        auto zone_container_copy = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*iter->second);
        _storage->emplace(iter->first, zone_container_copy);
    }
}

tchecker::clock_id_t const visited_map_t::no_of_virtual_clocks() {return this->_no_of_virtual_clocks;}

tchecker::strong_timed_bisim::visited_map_t::visited_map_storage_t::iterator visited_map_t::begin() {return _storage->begin();}

tchecker::strong_timed_bisim::visited_map_t::visited_map_storage_t::iterator visited_map_t::end() {return _storage->end();}

void visited_map_t::emplace(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second)
{
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));

  visited_map_key_t key = std::make_pair(std::make_pair(first->intval_ptr(), first->vloc_ptr()), std::make_pair(second->intval_ptr(), second->vloc_ptr()));
  auto common_virtual_constraint = tchecker::virtual_constraint::factory(first->zone(), _no_of_virtual_clocks);

  this->emplace(key, common_virtual_constraint);
}

void visited_map_t::emplace(visited_map_t &other)
{
  for(auto iter = other.begin(); iter != other.end(); ++iter) {

    for(auto vc = iter->second->begin(); vc != iter->second->end(); ++vc) {
      this->emplace(iter->first, *vc);
    }
  }
}

void visited_map_t::emplace(visited_map_key_t key, std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
{
  if (0 == _storage->count(key)) {
    auto new_zone_container = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*vc);
    _storage->emplace(key, new_zone_container);
  } else {
    (*_storage)[key]->append_zone(*vc);

    #if defined(SUBSETS_WITH_NEG_AND) || defined(SUBSETS_WITH_INTERSECTIONS) || defined(SUBSETS_WITH_COMPRESS) // in zone_container.hh
        (*_storage)[key]->compress();
    #endif
  }
}

bool visited_map_t::contains_superset(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second) 
{ 
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));

  visited_map_key_t key = std::make_pair(std::make_pair(first->intval_ptr(), first->vloc_ptr()), std::make_pair(second->intval_ptr(), second->vloc_ptr()));
  auto common_virtual_constraint = tchecker::virtual_constraint::factory(first->zone(), _no_of_virtual_clocks);

  if(0 == (*_storage).count(key))
      return false;

  return (*_storage)[key]->is_superset(*common_virtual_constraint); 
}

} // end of namespace strong_timed_bisim

} // end of namespace tchecker