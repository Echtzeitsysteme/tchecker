/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
*/

#include "tchecker/strong-timed-bisim/non_bisim_cache.hh"
#include "tchecker/vcg/virtual_constraint.hh"


namespace tchecker {

namespace strong_timed_bisim {

non_bisim_cache_t::non_bisim_cache_t(tchecker::clock_id_t no_of_virtual_clocks) : 
_no_of_virtual_clocks(no_of_virtual_clocks), _storage(std::make_shared<storage_t>()), _no_of_entries(0){ }

void non_bisim_cache_t::emplace(
  tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second, 
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> con)
{
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));
  map_key_t key = std::make_pair(std::make_pair(first->intval_ptr(), first->vloc_ptr()), std::make_pair(second->intval_ptr(), second->vloc_ptr()));

  con->compress();

  if((*_storage)[key] == nullptr || (*_storage)[key]->is_empty()) {
    (*_storage)[key] = tchecker::virtual_constraint::combine(*con, _no_of_virtual_clocks);
    ((*_storage)[key])->compress();
    return;
  }

  //std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> already_contained = (*_storage)[key];

  //already_contained->append_container(con);
  //already_contained->compress();
  //(*_storage)[key] = tchecker::virtual_constraint::combine(*already_contained, _no_of_virtual_clocks);
  ((*_storage)[key])->append_container(con);
  ((*_storage)[key])->compress();

  _no_of_entries++;

}

std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
non_bisim_cache_t::already_cached(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second)
{
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));
  auto vc = tchecker::virtual_constraint::factory(first->zone(), _no_of_virtual_clocks);

  map_key_t key = std::make_pair(std::make_pair(first->intval_ptr(), first->vloc_ptr()), std::make_pair(second->intval_ptr(), second->vloc_ptr()));
  if((*_storage)[key] == nullptr || (*_storage)[key]->is_empty()) {
    return std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_no_of_virtual_clocks + 1);
  }

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> already_found = (*_storage)[key];

  auto result = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_no_of_virtual_clocks + 1);
  auto intersection = tchecker::virtual_constraint::factory(_no_of_virtual_clocks);

  for(auto iter = already_found->begin(); iter != already_found->end(); ++iter) {
    vc->logic_and(intersection, **iter);
    if(intersection->is_fulfillable()) {
      result->append_zone(intersection);
    }
  }

  return result;
}


} // end of namespace strong_timed_bisim

} // end of namespace tchecker