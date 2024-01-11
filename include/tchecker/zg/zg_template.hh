/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_TEMPLATE_HH
#define TCHECKER_ZG_TEMPLATE_HH

#include <queue>

#include "tchecker/ta/ta.hh"
#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/extrapolation/extrapolation_factory.hh"
#include "tchecker/zg/semantics.hh"
#include "tchecker/zg/allocators.hh"

/*!
 \file zg_template.hh
 \brief Template for different kinds of zone graphs
 */

namespace tchecker {

namespace zg {

// Initial edges

/*!
 \brief Type of iterator over initial states
 */
using initial_iterator_t = tchecker::ta::initial_iterator_t;

/*!
\brief Type of range of iterators over inital states
*/
using initial_range_t = tchecker::ta::initial_range_t;

/*!
 \brief Dereference type for iterator over initial states
 */
using initial_value_t = tchecker::ta::initial_value_t;


// Final edges

/*!
 \brief Type of iterator over final edges
 \note this iterator ranges over the set of tuples of process locations and bounded integer variables valuations
 in the given system, the size of which is exponential in the number of locations and processes, as well as in the
 domains of bounded integer variables
 */
using final_iterator_t = tchecker::ta::final_iterator_t;

/*!
\brief Type of range of iterators over final edges
*/
using final_range_t = tchecker::ta::final_range_t;

/*!
 \brief Dereference type for iterator over final edges
 */
using final_value_t = tchecker::ta::final_value_t;

// Outgoing edges

/*!
 \brief Type of iterator over outgoing edges
 */
using outgoing_edges_iterator_t = tchecker::ta::outgoing_edges_iterator_t;

/*!
\brief Type of range of outgoing edges
*/
using outgoing_edges_range_t = tchecker::ta::outgoing_edges_range_t;

/*!
 \brief Type of outgoing vedge (range of synchronized/asynchronous edges)
 */
using outgoing_edges_value_t = tchecker::ta::outgoing_edges_value_t;


// Incoming edges

/*!
 \brief Type of iterator over incoming edges
 \note this iterator ranges over the set of tuple of process locations and bounded integer
 variables valuations in the given system, the size of which is exponential in the number
 of locations and processes, as well as in the domains of bounded integer variables
 */
using incoming_edges_iterator_t = tchecker::ta::incoming_edges_iterator_t;

/*!
\brief Type of range of incoming edges
*/
using incoming_edges_range_t = tchecker::ta::incoming_edges_range_t;

/*!
 \brief Type of incoming tuple of edges (range of synchronized/asynchronous edges)
 */
using incoming_edges_value_t = tchecker::ta::incoming_edges_value_t;

/*!
 \class zg_helper_t
 \brief Transition system of the zone graph over system of timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
template<typename Tused_state_sptr, typename Tused_const_state_sptr,
         typename Tused_transition_sptr, typename Tused_const_transition_sptr>
class zg_help_t : public tchecker::ts::fwd_t<Tused_state_sptr, Tused_const_state_sptr,
                                              Tused_transition_sptr, Tused_const_transition_sptr>,
                   public tchecker::ts::bwd_t<Tused_state_sptr, Tused_const_state_sptr,
                                              Tused_transition_sptr, Tused_const_transition_sptr>,
                   public tchecker::ts::fwd_impl_t<Tused_state_sptr, Tused_const_state_sptr,
                                                   Tused_transition_sptr, Tused_const_transition_sptr,
                                                   tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                                   tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>,
                   public tchecker::ts::bwd_impl_t<Tused_state_sptr, Tused_const_state_sptr,
                                                   Tused_transition_sptr, Tused_const_transition_sptr,
                                                   tchecker::zg::final_range_t, tchecker::zg::incoming_edges_range_t,
                                                   tchecker::zg::final_value_t, tchecker::zg::incoming_edges_value_t>,
                   public tchecker::ts::builder_t<Tused_state_sptr, Tused_transition_sptr>,
                   public tchecker::ts::inspector_t<Tused_const_state_sptr, Tused_const_transition_sptr>,
                   public tchecker::ts::sharing_t<Tused_state_sptr, Tused_transition_sptr> {

public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<Tused_state_sptr, Tused_const_state_sptr,
                                    Tused_transition_sptr, Tused_const_transition_sptr>;
  using bwd_t = tchecker::ts::bwd_t<Tused_state_sptr, Tused_const_state_sptr,
                                    Tused_transition_sptr, Tused_const_transition_sptr>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<Tused_state_sptr, Tused_const_state_sptr,
                                              Tused_transition_sptr, Tused_const_transition_sptr,
                                              tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                              tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>;
  using bwd_impl_t = tchecker::ts::bwd_impl_t<Tused_state_sptr, Tused_const_state_sptr,
                                              Tused_transition_sptr, Tused_const_transition_sptr,
                                              tchecker::zg::final_range_t, tchecker::zg::incoming_edges_range_t,
                                              tchecker::zg::final_value_t, tchecker::zg::incoming_edges_value_t>;
  using builder_t = tchecker::ts::builder_t<Tused_state_sptr, Tused_transition_sptr>;
  using inspector_t = tchecker::ts::inspector_t<Tused_const_state_sptr, Tused_const_transition_sptr>;
  using sharing_t = tchecker::ts::sharing_t<Tused_state_sptr, Tused_transition_sptr>;


  using sst_t = typename fwd_t::sst_t;
  using state_t = typename fwd_t::state_t;
  using const_state_t = typename fwd_t::const_state_t;
  using transition_t = typename fwd_t::transition_t;
  using const_transition_t = typename fwd_t::const_transition_t;
  using initial_range_t = typename fwd_impl_t::initial_range_t;
  using initial_value_t = typename fwd_impl_t::initial_value_t;
  using outgoing_edges_range_t = typename fwd_impl_t::outgoing_edges_range_t;
  using outgoing_edges_value_t = typename fwd_impl_t::outgoing_edges_value_t;
  using final_range_t = typename bwd_impl_t::final_range_t;
  using final_value_t = typename bwd_impl_t::final_value_t;
  using incoming_edges_range_t = typename bwd_impl_t::incoming_edges_range_t;
  using incoming_edges_value_t = typename bwd_impl_t::incoming_edges_value_t;


  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param sharing_type : type of state/transition sharing
   \param semantics : a zone semantics
   \param extrapolation : a zone extrapolation
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated automatically
   */
  zg_help_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
       std::shared_ptr<tchecker::zg::semantics_t> const & semantics,
       std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size)
    : _system(system), _sharing_type(sharing_type), _semantics(semantics), _extrapolation(extrapolation),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), block_size,
                       _system->clocks_count(tchecker::VK_FLATTENED) + 1, table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
  {
  }

  /*!
   \brief Copy constructor (deleted)
   */
  zg_help_t(zg_help_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  zg_help_t(zg_help_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~zg_help_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  zg_help_t & operator=(zg_help_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  zg_help_t & operator=(zg_help_t &&) = delete;

  using fwd_t::state;
  using fwd_t::status;
  using fwd_t::transition;

  // Forward

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual initial_range_t initial_edges()
  {
    return tchecker::ta::initial_edges(*_system);
  }

  /*!
   \brief Initial state and transition from an initial edge
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void initial(initial_value_t const & init_edge, std::vector<sst_t> & v,
                       tchecker::state_status_t mask = tchecker::STATE_OK)
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

  /*!
   \brief Initial state and transition with selected status
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    tchecker::ts::initial(*this, v, mask);
  }

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual outgoing_edges_range_t outgoing_edges(Tused_const_state_sptr const & s)
  {
    return tchecker::ta::outgoing_edges(*_system, s->vloc_ptr());
  }

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \param mask : mask on next states
   \post triples (status, s', t') have been added to v, for each transition s -t'-> s' along outgoing
   edge out_edge such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void next(Tused_const_state_sptr const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK)
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

    sst_handler<outgoing_edges_value_t const, bool>(out_edge, v, mask, ta_func, se_func, s, true);
  }


  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  */
  virtual void next(Tused_const_state_sptr const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    tchecker::ts::next(*this, s, v, mask);
  }

  // Backward

  /*!
   \brief Accessor
   \param labels : final states labels
   \return Final edges
   \note the return range goes across all tuples of process locations and bounded integer variables
   valuations in the given system. The number of such tuples is exponential in the number of locations
   and processes, as well as in the domains of bounded integer variables
   */
  virtual final_range_t final_edges(boost::dynamic_bitset<> const & labels)
  {
    return tchecker::ta::final_edges(*_system, labels);;
  }

  /*!
   \brief Final states and transitions from a final edge
   \param final_edge : final edge valuation
   \param v : container
   \param mask : mask on initial states
   \post triples (status, s, t) have been added to v, for each final state s and transition t
   such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void final(final_value_t const & final_edge, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK)
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

  /*!
  \brief Final states and transitions with selected status
  \param labels : set of final labels
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, states and transitions that satisfy
  labels, and such that status matches mask (i.e. status & mask != 0), have been
  pushed back into v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  \note complexity is exponential in the number of locations and processes, as well as in the doamins of
  bounded integer variables in the underlying system
  */
  virtual void final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    tchecker::ts::final(*this, labels, v, mask);
  }

  /*!
   \brief Accessor
   \param s : state
   \return incoming edges to state s
   \note the return range goes across all tuples of process locations and bounded integer variabkes
   valuations in the given system. The number of such tuples is exponential in the number of locations
   and processes, as well as the domains of bounded integer variables
   */
  virtual incoming_edges_range_t incoming_edges(Tused_const_state_sptr const & s)
  {
    return tchecker::ta::incoming_edges(*_system, s->vloc_ptr());
  }

  /*!
   \brief Previous state and transition from an incoming edge
   \param s : state
   \param in_edge : incoming edge value
   \param v : container
   \param mask : mask on initial states
   \post triples (status, s', t') have been added to v, for each incoming transition s'-t'->s
   along in_edge such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void prev(Tused_const_state_sptr const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK)
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

  /*!
  \brief Previous states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s' -t-> s is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  \note complexity is exponential in the number of locations and processes, as well as the domains of
  bounded integer valuations in the underlying system
  */
  virtual void prev(Tused_const_state_sptr const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    tchecker::ts::prev(*this, s, v, mask);
  }

  // Builder

  /*!
   \brief State/transition building from attributes
   \param attributes : a map of attributes
   \param v : container
   \param mask : mask on states
   \post all tuples (status, s, t) where s and t have been initialized from attributes,
   and status matches mask (i.e. status & mask != 0) have been pushed to v
   \pre see tchecker::zg::initialize
   \post a triple <status, s, t> has been pushed to v (if status matches mask), where the vector of
   locations in s has been initialized from attributes["vloc"], the integer valuation in s has been
   initialized from attributes["intval"], the zone in s has been initialized from attributes["zone"]
   and the invariant in the vector of locations in s, the vector of edges in t is empty and the source
   invariant in t has been initialized to the invariant in vloc
  */
  virtual void build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK)
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

  // split

  /*!
   \brief Split a state according to a clock constraint
   \param s : state
   \param c : clock constraint
   \param v : vector of states
   \pre the clock identifier in c are expressed over system clocks (with reference clock
   tchecker::REFCLOCK_ID)
   \post a copy of s has been added to v if it satisfies c or if it satisfies the negation of c
   otherwise, s has been splitted into s1, that satisfies c, and s2 that does not satisfy
   c, then s1 and s2 have been added to v
   */
  void split(Tused_const_state_sptr const & s, tchecker::clock_constraint_t const & c,
             std::vector<Tused_state_sptr> & v)
  {
    if (!tchecker::dbm::satisfies(s->zone().dbm(), s->zone().dim(), c))
      v.push_back(clone_and_constrain(s, -c));
    if (!tchecker::dbm::satisfies(s->zone().dbm(), s->zone().dim(), -c))
      v.push_back(clone_and_constrain(s, c));
  }


  /*!
   \brief Split a state according to a list of clock constraints
   \param s : state
   \param constraints : clock constraints
   \param v : vector of states
   \post s has been successively split w.r.t. every constraint in constraints. All resulting states
   have been added to v
   */
  void split(Tused_const_state_sptr const & s, tchecker::clock_constraint_container_t const & constraints,
             std::vector<Tused_state_sptr> & v)
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


  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(Tused_const_state_sptr const & s) const
  {
    return tchecker::ta::labels(*_system, *s);
  }

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(Tused_const_state_sptr const & s, std::map<std::string, std::string> & m) const
  {
    tchecker::ta::attributes(*_system, *s, m);
    m["zone"] = tchecker::to_string(s->zone(), _system->clock_variables().flattened().index());
  }

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(Tused_const_transition_sptr const & t, std::map<std::string, std::string> & m) const
  {
    tchecker::ta::attributes(*_system, *t, m);
  }

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if s has a non-empty zone, false otherwise
  */
  virtual bool is_valid_final(Tused_const_state_sptr const & s) const
  {
    return !s->zone().is_empty();
  }

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(Tused_const_state_sptr const & s) const
  {
    assert(s->zone_ptr()->dim() == _system->clocks_count(tchecker::VK_FLATTENED) + 1);
    return tchecker::ta::is_initial(*_system, *s) && tchecker::dbm::contains_zero(s->zone_ptr()->dbm(), s->zone_ptr()->dim());
  }

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(Tused_state_sptr & s)
  {
    _state_allocator.share(s);
  }

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(Tused_transition_sptr & t)
  {
    _transition_allocator.share(t);
  }

  /*!
   \brief Accessor
   \return Pointer to underlying system of timed processes
  */
  inline std::shared_ptr<tchecker::ta::system_t const> system_ptr() const { return _system; }

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  inline tchecker::ta::system_t const & system() const { return *_system; }

  /*!
   \brief Accessor
   \return sharing type of this synchronized product
  */
  inline enum tchecker::ts::sharing_type_t sharing_type() const { return _sharing_type; }

  /*!
   \brief Accessor
   \return number of clocks
   */
  tchecker::clock_id_t clocks_count()
  {
    return _system->clocks_count(tchecker::VK_FLATTENED);
  }

protected:

  /*!
  \brief High-Order function to shorten the handling of states and transitions
  \param edge : the parameter for the ta function
   \param v : container
   \param mask : mask on states
   \param ta_func : the function regarding the TA
   \param se_func : the function regarding the semantics
   \param to_clone : the state to clone (if clone is false, this can be nullptr. otherwise it cant)
   \param clone : whether to_clone should be cloned or a new state shall be used
   \post the new sst was added to v
   */
  template<typename edge_t, typename helping_hand_t>
  void sst_handler( edge_t edge, std::vector<sst_t> & v, tchecker::state_status_t mask,
                    tchecker::state_status_t (*ta_func)(tchecker::ta::system_t const &,
                                                        Tused_state_sptr &,
                                                        Tused_transition_sptr &,
                                                        edge_t &,
                                                        helping_hand_t *),
                   tchecker::state_status_t (*se_func)(  tchecker::ta::system_t const &,
                                                         tchecker::zg::semantics_t &,
                                                         tchecker::dbm::db_t *,
                                                         tchecker::clock_id_t,
                                                         Tused_state_sptr &,
                                                         Tused_transition_sptr &,
                                                         helping_hand_t *),
                  Tused_const_state_sptr const & to_clone, bool clone
                  )
  {
    Tused_state_sptr s;
    if(clone) { // for some crazy reason, I cannot check for null here and, therefore, we need clone
      s = _state_allocator.clone(*to_clone);
    }
    else {
      s = _state_allocator.construct();
    }
    Tused_transition_sptr t = _transition_allocator.construct();

    helping_hand_t helper;

    tchecker::state_status_t status = ta_func(*_system, s, t, edge, &helper);

    if(tchecker::STATE_OK == status) {

      tchecker::dbm::db_t * dbm = s->zone_ptr()->dbm();
      tchecker::clock_id_t dim = s->zone_ptr()->dim();

      status = se_func(*_system, *_semantics, dbm, dim, s, t, &helper);

      if(tchecker::STATE_OK == status) {
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
  /*!
   \brief Clone and constrain a state
   \param s : a state
   \param c : a clock constraint
   \return a clone of s with its zone intersected with c
   */
  Tused_state_sptr clone_and_constrain(Tused_const_state_sptr const & s,
                                                 tchecker::clock_constraint_t const & c)
  {
    tchecker::zg::state_sptr_t clone_s = _state_allocator.clone(*s);
    tchecker::dbm::constrain(clone_s->zone_ptr()->dbm(), clone_s->zone_ptr()->dim(), c);
    if (!clone_s->zone().is_empty() && _sharing_type == tchecker::ts::SHARING)
      share(clone_s);
    return clone_s;
  }

  /*!
   \brief Clone and constrain a state
   \param s : a state
   \param c : a vector of clock constraints
   \return a clone of s with its zone intersected with c
   */
  Tused_state_sptr clone_and_constrain(Tused_const_state_sptr const & s,
                                                 clock_constraint_container_t const & c)
  {
    tchecker::zg::state_sptr_t clone_s = _state_allocator.clone(*s);

    tchecker::dbm::constrain(clone_s->zone_ptr()->dbm(), clone_s->zone_ptr()->dim(), c);

    if (!clone_s->zone().is_empty() && _sharing_type == tchecker::ts::SHARING)
      share(clone_s);
    return clone_s;
  }

  std::shared_ptr<tchecker::ta::system_t const> _system;           /*!< System of timed processes */
  enum tchecker::ts::sharing_type_t _sharing_type;                 /*!< Sharing of state/transition components */
  std::shared_ptr<tchecker::zg::semantics_t> _semantics;           /*!< Zone semantics */
  std::shared_ptr<tchecker::zg::extrapolation_t> _extrapolation;   /*!< Zone extrapolation */
  tchecker::zg::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::zg::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

/* tools */

/*!
 \brief Compute initial state of a zone graph from a tuple of locations
 \param zg : zone graph
 \param vloc : tuple of locations
 \param mask : state status mask
 \return the initial state of zg with tuple of locations vloc and status
 compatible with mask if any, nullptr otherwise
 */
template<typename Tused_state_sptr, typename Tused_const_state_sptr,
         typename Tused_transition_sptr, typename Tused_const_transition_sptr>
Tused_state_sptr
initial(tchecker::zg::zg_help_t<Tused_state_sptr, Tused_const_state_sptr, Tused_transition_sptr, Tused_const_transition_sptr> & zg,
        tchecker::vloc_t const & vloc, tchecker::state_status_t mask = tchecker::STATE_OK)
{
  std::vector<typename tchecker::zg::zg_help_t<Tused_state_sptr, Tused_const_state_sptr, Tused_transition_sptr, Tused_const_transition_sptr>::sst_t> v;
  zg.initial(v, mask);
  for (auto && [status, s, t] : v) {
    if (s->vloc() == vloc)
      return s;
  }
  return nullptr;
}

/*!
 \brief Compute next state and transition from a tuple of edges
 \param zg : zone graph
 \param s : state
 \param vedge : tuple of edges
 \param mask : next state status mask
 \return the pair (nexts, nextt) of successor state and transition of s
 along tuple of edges vedge if any, (nullptr, nullptr) otherwise
 */
template<typename Tused_state_sptr, typename Tused_const_state_sptr,
         typename Tused_transition_sptr, typename Tused_const_transition_sptr>
std::tuple<Tused_state_sptr, Tused_transition_sptr>
next(tchecker::zg::zg_help_t<Tused_state_sptr, Tused_const_state_sptr, Tused_transition_sptr, Tused_const_transition_sptr> &zg,
     tchecker::zg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge, tchecker::state_status_t mask = tchecker::STATE_OK)
{
  std::vector<typename tchecker::zg::zg_help_t<Tused_state_sptr, Tused_const_state_sptr, Tused_transition_sptr, Tused_const_transition_sptr>::sst_t> v;
  zg.next(s, v, mask);
  for (auto && [status, nexts, nextt] : v)
    if (nextt->vedge() == vedge)
      return std::make_tuple(nexts, nextt);
  return std::make_tuple(nullptr, nullptr);
}

/* factory */

template<typename Tzg>
std::shared_ptr<Tzg> factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             std::size_t table_size)
{
  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
      tchecker::zg::extrapolation_factory(extrapolation_type, *system)};
  if (extrapolation.get() == nullptr)
    return nullptr;
  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};
  return std::make_shared<Tzg>(system, sharing_type, semantics, extrapolation, block_size, table_size);
}

template<typename Tzg>
std::shared_ptr<Tzg> factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                             std::size_t table_size)
{
  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
      tchecker::zg::extrapolation_factory(extrapolation_type, clock_bounds)};
  if (extrapolation.get() == nullptr)
    return nullptr;
  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};
  return std::make_shared<Tzg>(system, sharing_type, semantics, extrapolation, block_size, table_size);
}

} // end of namespace zg

} // end of namespace tchecker

#endif
