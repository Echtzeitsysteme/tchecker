/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zg/zg.hh"

#include <queue>

/* zg_t public functions */

namespace tchecker {

namespace zg {

initial_range_t zg_t::initial_edges()
{
  return tchecker::ta::initial_edges(*_system);
}

void zg_t::initial(initial_value_t const & init_edge, std::vector<sst_t> & v,
                   tchecker::state_status_t mask)
{
  auto ta_func = [](auto system, auto s, auto t, auto edge, auto unused) {
                      return tchecker::ta::initial(system, s->vloc_ptr(), s->intval_ptr(),
                                                   t->vedge_ptr(), t->src_invariant_container(), edge);
                    };

  auto se_func = [](auto system, auto semantics, auto dbm, auto dim, auto s, auto t, auto unused) {
                      bool delay_allowed = tchecker::ta::delay_allowed(system, *(s->vloc_ptr()));
                      return semantics.initial(dbm, dim, delay_allowed, t->src_invariant_container());
                    };

  sst_handler<initial_value_t const, bool>(init_edge, v, mask, ta_func, se_func, nullptr, false);
}

void zg_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::initial(*this, v, mask);
}

outgoing_edges_range_t zg_t::outgoing_edges(tchecker::zg::const_state_sptr_t const & s)
{
  return tchecker::ta::outgoing_edges(*_system, s->vloc_ptr());
}

void zg_t::next(tchecker::zg::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                tchecker::state_status_t mask)
{

  auto ta_func = [](auto system, auto s, auto t, auto edge, auto src_delay) {
                      *src_delay = tchecker::ta::delay_allowed(system, *(s->vloc_ptr()));
                      return tchecker::ta::next(system, s->vloc_ptr(), s->intval_ptr(),
                                                t->vedge_ptr(), t->src_invariant_container(), t->guard_container(),
                                                t->reset_container(), t->tgt_invariant_container(), edge);
                    };

  auto se_func = [](  auto system, auto semantics, auto dbm, auto dim, auto s, auto t, auto src_delay) {
                      bool tgt_delay = tchecker::ta::delay_allowed(system, *(s->vloc_ptr()));
                      return semantics.next(dbm, dim, *src_delay, t->src_invariant_container(), t->guard_container(),
                                            t->reset_container(), tgt_delay, t->tgt_invariant_container());
                    };

  sst_handler<outgoing_edges_value_t const, bool>(out_edge, v, mask, ta_func, se_func, s, true, _enable_extrapolation);
}

void zg_t::next(tchecker::zg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

final_range_t zg_t::final_edges(boost::dynamic_bitset<> const & labels)
{
  return tchecker::ta::final_edges(*_system, labels);
}

void zg_t::final(final_value_t const & final_edge, std::vector<sst_t> & v,
                 tchecker::state_status_t mask)
{
  auto ta_func = [](auto system, auto s, auto t, auto edge, auto unused) {
                      return tchecker::ta::final(system, s->vloc_ptr(), s->intval_ptr(),
                                                 t->vedge_ptr(), t->src_invariant_container(), edge);
                    };

  auto se_func = [](  auto system, auto semantics, auto dbm, auto dim, auto s, auto t, auto unused) {
                      bool delay = tchecker::ta::delay_allowed(system, *(s->vloc_ptr()));
                      return semantics.final(dbm, dim, delay, t->src_invariant_container());
                    };

  sst_handler<final_value_t const, bool>(final_edge, v, mask, ta_func, se_func, nullptr, false);
}

void zg_t::final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v,
           tchecker::state_status_t mask)
{
  tchecker::ts::final(*this, labels, v, mask);
}

incoming_edges_range_t zg_t::incoming_edges(tchecker::zg::const_state_sptr_t const & s)
{
  return tchecker::ta::incoming_edges(*_system, s->vloc_ptr());
}

void zg_t::prev(tchecker::zg::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                tchecker::state_status_t mask)
{
  auto ta_func = [](auto system, auto s, auto t, auto edge, auto tgt_delay) {
                      *tgt_delay = tchecker::ta::delay_allowed(system, *(s->vloc_ptr()));
                      return tchecker::ta::prev(system, s->vloc_ptr(), s->intval_ptr(), t->vedge_ptr(),
                                                t->src_invariant_container(), t->guard_container(),
                                                t->reset_container(), t->tgt_invariant_container(), edge);
                    };

  auto se_func = [](  auto system, auto semantics, auto dbm, auto dim, auto s, auto t, auto tgt_delay) {
                      bool src_delay = tchecker::ta::delay_allowed(system, *(s->vloc_ptr()));
                      return semantics.prev(dbm, dim, src_delay, t->src_invariant_container(), t->guard_container(),
                                            t->reset_container(), *tgt_delay, t->tgt_invariant_container());
                    };

  sst_handler<incoming_edges_value_t const, bool>(in_edge, v, mask, ta_func, se_func, s, true);
}

void zg_t::prev(tchecker::zg::const_state_sptr_t const & s, std::vector<sst_t> & v,
          tchecker::state_status_t mask)
{
  tchecker::ts::prev(*this, s, v, mask);
}

void zg_t::build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
               tchecker::state_status_t mask)
{
  auto ta_func = [](auto system, auto s, auto t, auto attributes, auto clk_constraints) {
                      auto state_status = tchecker::ta::initialize(system, s->vloc_ptr(), s->intval_ptr(),
                                                                   t->vedge_ptr(), t->src_invariant_container(), attributes);
                      if (state_status != STATE_OK)
                        return state_status;

                      // initialize zone from attributes["zone"]
                      try {
                        tchecker::from_string(*clk_constraints, system.clock_variables(), attributes.at("zone"));
                      }
                      catch (...) {
                        return tchecker::STATE_BAD;
                      }
                      return tchecker::STATE_OK;
                    };

  auto se_func = [](auto system, auto semantics, auto dbm, auto dim, auto s, auto t, auto clk_constraints) {
                      tchecker::dbm::universal_positive(dbm, dim);
                      tchecker::dbm::status_t zone_status = tchecker::dbm::constrain(dbm, dim, *clk_constraints);
                        if (zone_status == tchecker::dbm::EMPTY)
                          return tchecker::STATE_BAD;

                      // Apply invariant
                      zone_status = tchecker::dbm::constrain(dbm, dim, t->src_invariant_container());
                      if (zone_status == tchecker::dbm::EMPTY)
                        return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
                      return tchecker::STATE_OK;
                    };

  sst_handler<std::map<std::string, std::string> const &, tchecker::clock_constraint_container_t>(attributes, v, mask, ta_func, se_func, nullptr, false);
}

void zg_t::split(tchecker::zg::const_state_sptr_t const & s, tchecker::clock_constraint_t const & c,
           std::vector<tchecker::zg::state_sptr_t> & v)
{
  if (!tchecker::dbm::satisfies(s->zone().dbm(), s->zone().dim(), c))
    v.push_back(clone_and_constrain(s, -c));
  if (!tchecker::dbm::satisfies(s->zone().dbm(), s->zone().dim(), -c))
    v.push_back(clone_and_constrain(s, c));
}

void zg_t::split(tchecker::zg::const_state_sptr_t const & s, tchecker::clock_constraint_container_t const & constraints,
           std::vector<tchecker::zg::state_sptr_t> & v)
{
  std::vector<tchecker::zg::state_sptr_t> done;
  std::queue<tchecker::zg::state_sptr_t> todo;

  todo.push(_state_allocator.clone(*s));
  for (tchecker::clock_constraint_t const & c : constraints) {
    while (!todo.empty()) {
      split(tchecker::zg::const_state_sptr_t{todo.front()}, c, done);
      todo.pop();
    }
    for (tchecker::zg::state_sptr_t const & split_s : done)
      todo.push(split_s);
    done.clear();
  }

  for (; !todo.empty(); todo.pop())
    v.push_back(todo.front());
}

boost::dynamic_bitset<> zg_t::labels(tchecker::zg::const_state_sptr_t const & s) const
{
  return tchecker::ta::labels(*_system, *s);
}

void zg_t::attributes(tchecker::zg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(*_system, *s, m);
  m["zone"] = tchecker::to_string(s->zone(), _system->clock_variables().flattened().index());
}

void zg_t::attributes(tchecker::zg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(*_system, *t, m);
}

bool zg_t::is_valid_final(tchecker::zg::const_state_sptr_t const & s) const
{
  return !s->zone().is_empty();
}

bool zg_t::is_initial(tchecker::zg::const_state_sptr_t const & s) const
{
  assert(s->zone_ptr()->dim() == _system->clocks_count(tchecker::VK_FLATTENED) + 1);
  return tchecker::ta::is_initial(*_system, *s) && tchecker::dbm::contains_zero(s->zone_ptr()->dbm(), s->zone_ptr()->dim());
}

void zg_t::share(tchecker::zg::state_sptr_t & s)
{
  _state_allocator.share(s);
}

void zg_t::share(tchecker::zg::transition_sptr_t & t)
{
  _transition_allocator.share(t);
}


/* zg_t private functions */

template<typename edge_t, typename helping_hand_t>
void zg_t::sst_handler( edge_t edge, std::vector<sst_t> & v, tchecker::state_status_t mask,
                    tchecker::state_status_t (*ta_func)(tchecker::ta::system_t const &,
                                                        tchecker::zg::state_sptr_t &,
                                                        tchecker::zg::transition_sptr_t &,
                                                        edge_t &,
                                                        helping_hand_t *),
                   tchecker::state_status_t (*se_func)(  tchecker::ta::system_t const &,
                                                         tchecker::zg::semantics_t &,
                                                         tchecker::dbm::db_t *,
                                                         tchecker::clock_id_t,
                                                         tchecker::zg::state_sptr_t &,
                                                        tchecker::zg::transition_sptr_t &,
                                                         helping_hand_t *),
                  tchecker::zg::const_state_sptr_t const & to_clone, bool clone, bool enable_extrapolation
                  )
{
  tchecker::zg::state_sptr_t s;
  if(clone) { // for some crazy reasons, I cannot check for null here and, therefore, we need clone
    s = _state_allocator.clone(*to_clone);
  }
  else {
    s = _state_allocator.construct();
  }
  tchecker::zg::transition_sptr_t t = _transition_allocator.construct();

  helping_hand_t helper;

  tchecker::state_status_t status = ta_func(*_system, s, t, edge, &helper);

  if(tchecker::STATE_OK == status) {

    tchecker::dbm::db_t * dbm = s->zone_ptr()->dbm();
    tchecker::clock_id_t dim = s->zone_ptr()->dim();

    status = se_func(*_system, *_semantics, dbm, dim, s, t, &helper);

    if(tchecker::STATE_OK == status && enable_extrapolation) {
      _extrapolation->extrapolate(dbm, dim, *(s->vloc_ptr()));
    }
  }
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}



tchecker::zg::state_sptr_t zg_t::clone_and_constrain(tchecker::zg::const_state_sptr_t const & s,
                                                     tchecker::clock_constraint_t const & c)
{
  tchecker::zg::state_sptr_t clone_s = _state_allocator.clone(*s);
  tchecker::dbm::constrain(clone_s->zone_ptr()->dbm(), clone_s->zone_ptr()->dim(), c);
  if (!clone_s->zone().is_empty() && _sharing_type == tchecker::ts::SHARING)
    share(clone_s);
  return clone_s;
}

tchecker::zg::state_sptr_t zg_t::clone_and_constrain(tchecker::zg::const_state_sptr_t const & s,
                                                     clock_constraint_container_t const & c)
{
  tchecker::zg::state_sptr_t clone_s = _state_allocator.clone(*s);

  tchecker::dbm::constrain(clone_s->zone_ptr()->dbm(), clone_s->zone_ptr()->dim(), c);

  if (!clone_s->zone().is_empty() && _sharing_type == tchecker::ts::SHARING)
    share(clone_s);
  return clone_s;
}


/* tools */


tchecker::zg::state_sptr_t
initial(tchecker::zg::zg_t & zg,
        tchecker::vloc_t const & vloc, tchecker::state_status_t mask)
{
  std::vector<typename tchecker::zg::zg_t::sst_t> v;
  zg.initial(v, mask);
  for (auto && [status, s, t] : v) {
    if (s->vloc() == vloc)
      return s;
  }
  return nullptr;
}

std::tuple<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>
next(tchecker::zg::zg_t &zg,
     tchecker::zg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge, tchecker::state_status_t mask)
{
  std::vector<typename tchecker::zg::zg_t::sst_t> v;
  zg.next(s, v, mask);
  for (auto && [status, nexts, nextt] : v)
    if (nextt->vedge() == vedge)
      return std::make_tuple(nexts, nextt);
  return std::make_tuple(nullptr, nullptr);
}


/* factory */


std::shared_ptr<zg_t> factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             std::size_t table_size, bool enable_extrapolation)
{
  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
      tchecker::zg::extrapolation_factory(extrapolation_type, *system)};
  if (extrapolation.get() == nullptr)
    return nullptr;
  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};
  return std::make_shared<zg_t>(system, sharing_type, semantics, extrapolation, block_size, table_size, enable_extrapolation);
}

std::shared_ptr<zg_t> factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                             std::size_t table_size, bool enable_extrapolation)
{
  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
      tchecker::zg::extrapolation_factory(extrapolation_type, clock_bounds)};
  if (extrapolation.get() == nullptr)
    return nullptr;
  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};
  return std::make_shared<zg_t>(system, sharing_type, semantics, extrapolation, block_size, table_size, enable_extrapolation);
}

} // end of namespace zg

} // end of namespace tchecker
