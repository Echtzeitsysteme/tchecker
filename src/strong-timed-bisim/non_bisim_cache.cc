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

namespace non_bisim_cache {

non_bisim_cache_t::non_bisim_cache_t(tchecker::clock_id_t no_of_virtual_clocks, bool generate_strategy) : 
_no_of_virtual_clocks(no_of_virtual_clocks), _storage(std::make_shared<storage_t>()), _no_of_entries(0), _generate_strategy(generate_strategy){ }

void non_bisim_cache_t::emplace(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second, contradiction_t & con)
{

  assert(con.get_contradictions()->dim() - 1 == _no_of_virtual_clocks);

  // if we do not need to generate a strategy, we are allowed to store all contradictions within a single contradiction, which improves performance.
  // Therefore, in that case, we just set min_steps_to_cont to zero.
  if(!_generate_strategy) {
    con.set_min_steps_to_cont(0);
  }
  map_key_t key = std::make_pair(tchecker::ta::state_t(first->vloc_ptr(), first->intval_ptr()), tchecker::ta::state_t(second->vloc_ptr(), second->intval_ptr()));

  if(nullptr == (*_storage)[key] || (*_storage)[key]->empty()) {
    (*_storage)[key] = std::make_shared<std::vector<contradiction_t>>();
    contradiction_t to_add{con};
    (*_storage)[key]->emplace_back(to_add);
    _no_of_entries++;
    return;
  }

  bool con_is_added = false;

  for(auto& cur : *(*_storage)[key]) {
    if(cur.min_steps_to_cont() == con.min_steps_to_cont()) {
      if(!(*con.get_contradictions() <= *cur.get_contradictions())) {
        cur.add_contradiction(con);
      }
      con_is_added = true;
    }
    if(cur.min_steps_to_cont() < con.min_steps_to_cont() && *con.get_contradictions() <= *cur.get_contradictions()) {
      con.set_min_steps_to_cont(cur.min_steps_to_cont());
      con_is_added = true;
    }
    if(con.min_steps_to_cont() < cur.min_steps_to_cont() && *cur.get_contradictions() <= *con.get_contradictions()) {
      cur.set_min_steps_to_cont(con.min_steps_to_cont());
      emplace(first, second, con);
      return;
    }
  }

  if(!con_is_added) {
    ((*_storage)[key])->emplace_back(con);
  }

  cleanup_entry(key);
  
}

std::shared_ptr<contradiction_t>
non_bisim_cache_t::already_cached(tchecker::zg::state_sptr_t first, tchecker::zg::state_sptr_t second) const
{
  assert(first->zone().is_virtual_equivalent(second->zone(), _no_of_virtual_clocks));

  auto vc = tchecker::virtual_constraint::factory(first->zone(), _no_of_virtual_clocks);
  map_key_t key = std::make_pair(tchecker::ta::state_t(first->vloc_ptr(), first->intval_ptr()), tchecker::ta::state_t(second->vloc_ptr(), second->intval_ptr()));

  auto result = std::make_shared<contradiction_t>(_no_of_virtual_clocks);

  if((*_storage)[key] == nullptr) {
    return result;
  }

  for(auto cur : *(*_storage)[key]) {

    std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> already_found = cur.get_contradictions();

    auto new_container = std::make_shared<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>(_no_of_virtual_clocks + 1);
    auto intersection = tchecker::virtual_constraint::factory(_no_of_virtual_clocks);

    for(auto iter = already_found->begin(); iter != already_found->end(); ++iter) {
      vc->logic_and(intersection, **iter);
      if(intersection->is_fulfillable()) {
        new_container->append_zone(*intersection);
      }
    }

    new_container->compress();
    contradiction_t new_cont{new_container, cur.min_steps_to_cont()};
    result->add_contradiction(new_cont);
  }

  assert(std::all_of(result->get_contradictions()->begin(), result->get_contradictions()->end(), 
          [vc](std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> cont)
          {
            auto intersection = tchecker::virtual_constraint::factory(vc->get_no_of_virtual_clocks());
            vc->logic_and(intersection, *cont);
            if(!intersection->is_fulfillable()) {
              return false;
            }   
            
            return true;
          }));

  return result;
}

std::shared_ptr<std::vector<contradiction_t>>
non_bisim_cache_t::entry(tchecker::ta::state_t & first, tchecker::ta::state_t & second) const
{
  map_key_t key = std::make_pair(first, second);
  return entry(key);
}

std::shared_ptr<std::vector<contradiction_t>>
non_bisim_cache_t::entry(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair) const
{
  if((*_storage)[loc_pair] == nullptr) {
    return std::make_shared<std::vector<contradiction_t>>();
  }
  return (*_storage)[loc_pair];
}

bool non_bisim_cache_t::is_cached(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair, std::shared_ptr<tchecker::clockval_t> clockval, 
                                  tchecker::clock_id_t no_of_orig_clks_1, tchecker::clock_id_t no_of_orig_clks_2, bool first_not_second) const
{
  std::shared_ptr<std::vector<contradiction_t>> entries = entry(loc_pair);

  for(auto entry : *entries) {
    for(auto cur : *entry.get_contradictions()) {

      std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>> zones =
        cur->generate_synchronized_zones(no_of_orig_clks_1, no_of_orig_clks_2);

      assert((first_not_second) ? (zones.first->dim() == clockval->size()) : (zones.first->dim() == clockval->size()));

      if(  (first_not_second && zones.first->belongs(*clockval)) ||
           (!first_not_second && zones.second->belongs(*clockval))) {
        return true;
      }
    }
  }
  return false;
}


void non_bisim_cache_t::cleanup_entry(map_key_t & key)
{
  for(auto& cur : *(*_storage)[key]) {
    cur.get_contradictions()->compress();
  }

  for(auto outer = (*_storage)[key]->begin(); outer != (*_storage)[key]->end(); outer++) {
    for(auto inner = std::next(outer); inner != (*_storage)[key]->end(); inner++) {
      if(outer->min_steps_to_cont() == inner->min_steps_to_cont()) {
        outer->add_contradiction(*inner);
        (*_storage)[key]->erase(inner);
        cleanup_entry(key);
        return;
      }
    }
  }
}

} // end of namespace non_bisim_cache

} // end of namespace strong_timed_bisim

} // end of namespace tchecker