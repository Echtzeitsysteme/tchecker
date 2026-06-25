/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/strategy.hh"

#include "tchecker/ta/state.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace strategy {

/* states_to_check_entry_t */

state_to_check_t::state_to_check_t(const tchecker::vloc_t & vloc, const tchecker::intval_t & intval,
                                   tchecker::intrusive_shared_ptr_t<const tchecker::zg::shared_zone_t> zone)
{
  // Since there exist several problems with the copy of states from the reachability analysis to the comparison, we copy everything.
  // vloc
  _vloc_ptr = new tchecker::vloc_t(vloc.size());
  for(tchecker::clock_id_t i = 0; i < _vloc_ptr->size(); ++i) {
    (*_vloc_ptr)[i] = vloc[i];
  }

  // intval
  _intval_ptr = new tchecker::intval_t(intval.size());
  for(tchecker::clock_id_t i = 0; i < _intval_ptr->size(); ++i) {
    (*_intval_ptr)[i] = intval[i];
  }

  // zone
  _zone_ptr = tchecker::zg::zone_allocate_and_construct(zone->dim(), zone->dim());

  tchecker::dbm::copy(_zone_ptr->dbm(), zone->dbm(), _zone_ptr->dim());

}

state_to_check_t::state_to_check_t()
{
  _vloc_ptr = new tchecker::vloc_t(1);
  _intval_ptr = new tchecker::intval_t(1);
  _zone_ptr = tchecker::zg::zone_allocate_and_construct(1, 1);
}


state_to_check_t::state_to_check_t(state_to_check_t const & other)
{
  _vloc_ptr = new tchecker::vloc_t(other._vloc_ptr->size());
  for(tchecker::clock_id_t i = 0; i < _vloc_ptr->size(); ++i) {
    (*_vloc_ptr)[i] = (*other._vloc_ptr)[i];
  }

  // intval
  _intval_ptr = new tchecker::intval_t(other._intval_ptr->size());
  for(tchecker::clock_id_t i = 0; i < other._intval_ptr->size(); ++i) {
    (*_intval_ptr)[i] = (*other._intval_ptr)[i];
  }

  // zone
  _zone_ptr = tchecker::zg::zone_allocate_and_construct(other._zone_ptr->dim(), other._zone_ptr->dim());

  tchecker::dbm::copy(_zone_ptr->dbm(), other._zone_ptr->dbm(), _zone_ptr->dim());
}

state_to_check_t & state_to_check_t::operator=(state_to_check_t const & other)
{
  if (this != &other) {
    delete _vloc_ptr;
    delete _intval_ptr;
    tchecker::zg::zone_destruct_and_deallocate(_zone_ptr);

    _vloc_ptr = new tchecker::vloc_t(other._vloc_ptr->size());
    for(tchecker::clock_id_t i = 0; i < _vloc_ptr->size(); ++i) {
      (*_vloc_ptr)[i] = (*other._vloc_ptr)[i];
    }

    // intval
    _intval_ptr = new tchecker::intval_t(other._intval_ptr->size());
    for(tchecker::clock_id_t i = 0; i < other._intval_ptr->size(); ++i) {
      (*_intval_ptr)[i] = (*other._intval_ptr)[i];
    }

    // zone
    _zone_ptr = tchecker::zg::zone_allocate_and_construct(other._zone_ptr->dim(), other._zone_ptr->dim());

    tchecker::dbm::copy(_zone_ptr->dbm(), other._zone_ptr->dbm(), _zone_ptr->dim());
  }
  return *this;
}

state_to_check_t::state_to_check_t(state_to_check_t&& other) noexcept
 : _vloc_ptr(other._vloc_ptr), _intval_ptr(other._intval_ptr), _zone_ptr(other._zone_ptr)
{
  other._vloc_ptr = nullptr;
  other._intval_ptr = nullptr;
  other._zone_ptr = nullptr;
}

state_to_check_t& state_to_check_t::operator=(state_to_check_t&& other) noexcept
{
  if (this != &other) {
    delete _vloc_ptr;
    delete _intval_ptr;
    tchecker::zg::zone_destruct_and_deallocate(_zone_ptr);

    _vloc_ptr = other._vloc_ptr;
    _intval_ptr = other._intval_ptr;
    _zone_ptr = other._zone_ptr;

    other._vloc_ptr = nullptr;
    other._intval_ptr = nullptr;
    other._zone_ptr = nullptr;
  }
  return *this;
}

state_to_check_t::~state_to_check_t()
{
  delete _vloc_ptr;

  delete _intval_ptr;

  tchecker::zg::zone_destruct_and_deallocate(_zone_ptr);
}

/* strategy_t */

strategy_t::strategy_t(std::shared_ptr<tchecker::vcg::vcg_t> A, 
                       std::shared_ptr<tchecker::vcg::vcg_t> B,
                       std::vector<std::shared_ptr<tchecker::strong_timed_bisim::strategy::state_to_check_t>> & symbolic_states_to_check,
                       tchecker::clock_id_t first_vloc_size,
                       tchecker::clock_id_t second_vloc_size,
                       unsigned short first_intval_size,
                       unsigned short second_intval_size)
  : _A(A), _B(B),
    _first_vloc_size(first_vloc_size), _second_vloc_size(second_vloc_size),
    _first_intval_size(first_intval_size), _second_intval_size(second_intval_size)
{
  assert(!symbolic_states_to_check.empty());
  assert(symbolic_states_to_check[0]->vloc_ptr()->size() == _first_vloc_size + _second_vloc_size);
  assert(symbolic_states_to_check[0]->intval_ptr()->size() == _first_intval_size + _second_intval_size);

  for(auto to_add : symbolic_states_to_check) {
    std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> loc_pair = extract_location_pair(to_add->vloc_ptr(), to_add->intval_ptr());
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc 
      = tchecker::virtual_constraint::factory(*to_add->zone_ptr(), to_add->zone_ptr()->dim()-1);
    bool added = false;
    for(auto already_added : _states_to_check) {
      if(*already_added->loc_pair() == *loc_pair) {
        already_added->container().append_zone(vc);
        added = true;
        break;
      }
    }
    if(!added) {
      std::shared_ptr<entry_t> new_entry = std::make_shared<entry_t>(loc_pair, vc->dim());
      new_entry->append_vc(vc);
      _states_to_check.emplace_back(new_entry);
    }
  }

}

void strategy_t::insert_symb_states(std::shared_ptr<non_bisim_cache_t> non_bisim_cache, std::shared_ptr<visited_map_t> visited_map)
{
  assert(non_bisim_cache != nullptr);
  assert(visited_map != nullptr);
  assert(visited_map->no_of_virtual_clocks() == non_bisim_cache->no_of_virtual_clocks());
  _non_bisim_cache = non_bisim_cache;
  _visited_map = visited_map;
}

std::shared_ptr<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>> strategy_t::get_non_contained_states()
{
  assert(_non_bisim_cache != nullptr);
  assert(_visited_map != nullptr);

  std::shared_ptr<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>> result = nullptr;

  for(auto entry : _states_to_check) {
    for(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc : entry->container()) {
      result = get_non_contained_states(*entry->loc_pair(), vc);
      if(nullptr != result) {
        break;
      }
    }
    if(nullptr != result) {
      break;
    }
  }

  return result;
}

std::shared_ptr<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>
strategy_t::get_non_contained_states(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & loc_pair, 
                                     std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc)
{
  assert(_non_bisim_cache != nullptr);
  assert(_visited_map != nullptr);
  
  if(nullptr == vc) {
    return nullptr;
  }

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
    non_bisim = _non_bisim_cache->entry(loc_pair);

  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
    bisim = _visited_map->entry(loc_pair);

  tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> container{_visited_map->no_of_virtual_clocks() + 1};

  for(auto cur : *non_bisim) {
    container.append_zone(*cur);
  }

  for(auto cur : *bisim) {
    container.append_zone(*cur);
  }

  container.compress();
  container.remove_empty();

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> non_contained = find_non_contained(vc, container);

  if(nullptr == non_contained || non_contained->is_empty()) {
    return nullptr;
  }

  if(_A->get_urgent_or_committed() || _B->get_urgent_or_committed()) {
    // in this case, the dimension of non_contained (which includes the reference clock) is the same as the number of virtual clocks 
    // (and, therefore, one to small)
    assert(non_contained->dim() == _A->get_no_of_virtual_clocks());
    std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> tmp = tchecker::virtual_constraint::factory(_A->get_no_of_virtual_clocks());
    for(tchecker::clock_id_t i = 0; i < non_contained->dim(); ++i) {
      for(tchecker::clock_id_t j = 0; j < non_contained->dim(); ++j) {
        tchecker::dbm::db_t *tmp_entry = tchecker::dbm::access(tmp->dbm(), tmp->dim(), i, j);
        tchecker::dbm::db_t *non_contained_entry = tchecker::dbm::access(non_contained->dbm(), non_contained->dim(), i, j);
        *tmp_entry = *non_contained_entry;
      }
    }
    non_contained = tmp;
  }

  std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>>  synced_zones 
    = non_contained->generate_synchronized_zones(_A->get_no_of_original_clocks(), _B->get_no_of_original_clocks());
  
  auto result = std::make_shared<std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>>(
    _A->create_state(loc_pair.first.vloc(), loc_pair.first.intval(), *synced_zones.first),
    _B->create_state(loc_pair.second.vloc(), loc_pair.second.intval(), *synced_zones.second)
  );

  return result;
}

std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
strategy_t::find_non_contained(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> to_be_contained, 
                               tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & container)
{
  if(container.is_empty()) {
    if(tchecker::dbm::is_empty_0(to_be_contained->dbm(), to_be_contained->dim())) {
      return nullptr;
    } else {
      return to_be_contained;
    }
  }

  std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> cur
    = container[0];

  container.remove_first();

  /*
    The idea is as follows:
    We take the first element of container. 
    We take an element of the DBM and create the opposite clock constraint.
    We conjugate the clock constraint with to_be_contained.
    If the result is not empty, the result is not contained by the first element of container. Therefore, we check the next.

    This is done with each element of the DBM. However, we have to ignore the urgent or committed clock in case there exists one.
   */
  tchecker::clock_id_t dim = (_A->get_urgent_or_committed() || _B->get_urgent_or_committed()) ? (cur->dim() - 1) : (cur->dim());
  assert(dim == to_be_contained->dim());
  for(tchecker::clock_id_t i = 0; i < dim; ++i) {
    for(tchecker::clock_id_t j = 0; j < dim; ++j) {
      // copy to_be_contained
      std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> copy
        = tchecker::virtual_constraint::factory(*to_be_contained);

      // get the dbm entry and ...
      tchecker::dbm::db_t *dbm_entry = tchecker::dbm::access(cur->dbm(), cur->dim(), i, j);
      // ... create the inverted clock constraint (not(i - j cmp n) <=> (j-i !cmp -n))
      tchecker::clock_constraint_t constraint{
            (j == 0) ? (tchecker::REFCLOCK_ID) : (j-1),
            (i == 0) ? (tchecker::REFCLOCK_ID) : (i-1), 
            (tchecker::ineq_cmp_t::LE == dbm_entry->cmp) ? (tchecker::ineq_cmp_t::LT) : (tchecker::ineq_cmp_t::LE),
            static_cast<tchecker::integer_t>(-1*dbm_entry->value)};
      
      if(tchecker::dbm::NON_EMPTY == tchecker::dbm::constrain(copy->dbm(), copy->dim(), constraint)) {
        std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> non_contained = find_non_contained(copy, container);
        if(nullptr != non_contained && !tchecker::dbm::is_empty_0(non_contained->dbm(), non_contained->dim())) {
          return non_contained;
        }
      }
    }
  }

  return nullptr;
}


std::ostream & strategy_t::strategy_output(std::ostream & os)
{
  os << "Clock Ordering" << std::endl;
  os << "0 ";
  for(tchecker::clock_id_t i = 0; i < _A->get_no_of_original_clocks(); ++i) {
    os << _A->system().as_system_system().clock_name(i) << " ";
  }
  for(tchecker::clock_id_t i = 0; i < _B->get_no_of_original_clocks(); ++i) {
    os << _B->system().as_system_system().clock_name(i);
  }
  os << std::endl;
  output_non_bisim(os);
  os << std::endl;
  output_bisim(os);

  return os;
}

std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> 
strategy_t::extract_location_pair(tchecker::vloc_t * vloc,
                                  tchecker::intval_t * intval)
{

  tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> first_vloc
    = tchecker::make_shared_t<tchecker::vloc_t>::allocate_and_construct(_first_vloc_size);
  for(tchecker::clock_id_t i = 0; i < _first_vloc_size; ++i) {
    (*first_vloc)[i] = (*vloc)[i];
  }

  tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> first_intval
    = tchecker::make_shared_t<tchecker::intval_t>::allocate_and_construct(_first_intval_size);
  for(tchecker::clock_id_t i = 0; i < _first_intval_size; ++i) {
    (*first_intval)[i] = (*intval)[i];
  }

  tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> second_vloc
    = tchecker::make_shared_t<tchecker::vloc_t>::allocate_and_construct(_second_vloc_size);
  for(tchecker::clock_id_t i = 0; i < _second_vloc_size; ++i) {
    (*second_vloc)[i] = (*vloc)[i + _first_vloc_size];
  }

  tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> second_intval
    = tchecker::make_shared_t<tchecker::intval_t>::allocate_and_construct(_second_intval_size);
  for(tchecker::clock_id_t i = 0; i < _second_intval_size; ++i) {
    (*second_intval)[i] = (*intval)[i + _first_intval_size];
  }

  std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> result
    = std::make_shared<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>>(
          tchecker::ta::state_t(first_vloc, first_intval), tchecker::ta::state_t(second_vloc, second_intval));

  return result;
}

void strategy_t::output_non_bisim(std::ostream & os)
{
  os << "Non Bisim" << std::endl;
  std::shared_ptr<tchecker::strong_timed_bisim::storage_t> storage = _non_bisim_cache->storage();
  for(auto cur : *storage) {
    output_state_pair_with_container(os, cur.first, cur.second);
  }
}

void strategy_t::output_bisim(std::ostream & os)
{
  os << "Bisim" << std::endl;

  for(auto cur : *_visited_map) {
    output_state_pair_with_container(os, cur.first, cur.second);
  }
}

void strategy_t::output_state_pair_with_container(std::ostream & os, 
                                                  std::pair<tchecker::ta::state_t, tchecker::ta::state_t> loc_pair,
                                                  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> container)
{
  os << "First Location: (";
  for(auto loc : loc_pair.first.vloc()) {
    if(*loc_pair.first.vloc().begin() != loc) {
      os << ", ";
    }
    os << _A->system().as_system_system().location(loc);
  }
  os << ")" << std::endl;

  os << "Second Location: (";
  for(auto loc : loc_pair.second.vloc()) {
    if(*loc_pair.second.vloc().begin() != loc) {
      os << ", ";
    }
    os << _A->system().as_system_system().location(loc);
  }
  os << ")" << std::endl;

  for(auto vc : *container) {
    tchecker::clock_id_t dim = (_A->get_urgent_or_committed() || _B->get_urgent_or_committed()) ? vc->dim() - 1 : vc->dim();
    tchecker::dbm::output_matrix(os, vc->dbm(), dim);
    os << std::endl;
  }
}

} // end of namespace strategy

} // end of namespace strong_timed_bisim

} // end of namespace tcheckers