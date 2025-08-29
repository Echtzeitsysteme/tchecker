/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/visited_map.hh"
#include "tchecker/vcg/sync.hh"

namespace tchecker {

namespace strong_timed_bisim {

visited_map_t::visited_map_t(tchecker::clock_id_t no_of_virtual_clocks, std::shared_ptr<tchecker::vcg::vcg_t> A,
                             std::shared_ptr<tchecker::vcg::vcg_t> B)
    : _no_of_virtual_clocks(no_of_virtual_clocks), _storage(std::make_shared<visited_map_storage_t>()), _A(A), _B(B)
{
}

visited_map_t::visited_map_t(visited_map_t & t, std::shared_ptr<tchecker::vcg::vcg_t> A,
                             std::shared_ptr<tchecker::vcg::vcg_t> B)
    : _no_of_virtual_clocks(t.no_of_virtual_clocks()), _storage(std::make_shared<visited_map_storage_t>()), _A(A), _B(B)
{
  emplace(t);
}

tchecker::clock_id_t const visited_map_t::no_of_virtual_clocks() { return this->_no_of_virtual_clocks; }

tchecker::strong_timed_bisim::visited_map_t::visited_map_storage_t::iterator visited_map_t::begin()
{
  return _storage->begin();
}

tchecker::strong_timed_bisim::visited_map_t::visited_map_storage_t::iterator visited_map_t::end() { return _storage->end(); }

void visited_map_t::emplace(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second)
{
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));

  assert(tchecker::vcg::are_zones_synced(first->zone(), second->zone(), first->zone().dim() - _no_of_virtual_clocks - 1,
                                         second->zone().dim() - _no_of_virtual_clocks - 1));

  auto key_first = tchecker::ta::state_t(first->vloc_ptr(), first->intval_ptr());
  auto key_second = tchecker::ta::state_t(second->vloc_ptr(), second->intval_ptr());

  auto common_virtual_constraint = tchecker::virtual_constraint::factory(first->zone(), _no_of_virtual_clocks);

  this->emplace(std::make_pair<tchecker::ta::state_t, tchecker::ta::state_t>(std::move(key_first), std::move(key_second)), common_virtual_constraint);
}

void visited_map_t::emplace(visited_map_t & other)
{
  for (auto & [key, value] : other) {
    for (auto & cur : *value) {
      emplace(key, cur);
    }
  }
}

void visited_map_t::emplace(const visited_map_key_t key,
                            std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
{
  if (0 == _storage->count(key)) {
    auto new_zone_container =
        std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(*vc);
    _storage->emplace(key, new_zone_container);
  }
  else {
#if !defined(SUBSETS_WITH_NEG_AND) && !defined(SUBSETS_WITH_INTERSECTIONS) && !defined(SUBSETS_WITH_COMPRESS) // in zone_container.hh
    auto find = std::find_if((*_storage)[key]->begin(), (*_storage)[key]->end(), 
                    [vc](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> const & p){
                        return *p == *vc;
                    }
                );
    if(find == (*_storage)[key]->end()) {
      (*_storage)[key]->append_zone(*vc);
    }
#else
      (*_storage)[key]->append_zone(*vc);
#endif

#if defined(SUBSETS_WITH_NEG_AND) || defined(SUBSETS_WITH_INTERSECTIONS) || defined(SUBSETS_WITH_COMPRESS) // in zone_container.hh
    (*_storage)[key]->compress();
#endif
  }
}

bool visited_map_t::check_and_add_pair(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second)
{
  auto A_norm = _A->clone_state(first);
  auto B_norm = _B->clone_state(second);

  // normalizing, to check whether we have already seen this pair.
  tchecker::vloc_t * extrapolation_vloc =
      tchecker::vloc_allocate_and_construct((*(A_norm->vloc_ptr())).size() + (*(B_norm->vloc_ptr())).size(),
                                            (*(A_norm->vloc_ptr())).size() + (*(B_norm->vloc_ptr())).size());

  // get location maps for clocks of first TA for locations of first TA
  for (size_t i = 0; i < (*(A_norm->vloc_ptr())).size(); ++i) {
    (*extrapolation_vloc)[i] = (*(A_norm->vloc_ptr()))[i];
  }
  // get location maps for clocks of second TA for locations of second TA
  for (size_t i = 0; i < (*(B_norm->vloc_ptr())).size(); ++i) {
    (*extrapolation_vloc)[(*(A_norm->vloc_ptr())).size() + i] = _A->get_no_of_locations() + (*(B_norm->vloc_ptr()))[i];
  }

  _A->run_extrapolation(A_norm->zone().dbm(), A_norm->zone().dim(), *extrapolation_vloc);
  _B->run_extrapolation(B_norm->zone().dbm(), B_norm->zone().dim(), *extrapolation_vloc);

  vloc_destruct_and_deallocate(extrapolation_vloc);

  tchecker::dbm::tighten(A_norm->zone().dbm(), A_norm->zone().dim());
  tchecker::dbm::tighten(B_norm->zone().dbm(), B_norm->zone().dim());

  if (contains_superset(A_norm, B_norm)) {
    return true;
  }
  else {
    emplace(A_norm, B_norm);
    return false;
  }
}

bool visited_map_t::contains_superset(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second)
{
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));

  visited_map_key_t key = std::make_pair<tchecker::ta::state_t, tchecker::ta::state_t>(
      tchecker::ta::state_t(first->vloc_ptr(), first->intval_ptr()),
      tchecker::ta::state_t(second->vloc_ptr(), second->intval_ptr()));
      
  auto common_virtual_constraint = tchecker::virtual_constraint::factory(first->zone(), _no_of_virtual_clocks);

  if (0 == (*_storage).count(key))
    return false;

  return (*_storage)[key]->is_superset(*common_virtual_constraint);
}

} // end of namespace strong_timed_bisim

} // end of namespace tchecker