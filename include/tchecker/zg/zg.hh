/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_HH
#define TCHECKER_ZG_HH

#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/extrapolation/extrapolation_factory.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/ts/fwd.hh"
#include "tchecker/ts/bwd.hh"
#include "tchecker/zg/allocators.hh"
#include "tchecker/zg/semantics.hh"

/*!
 \file zg.hh
 \brief Zone graphs
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
class zg_t : public tchecker::ts::fwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>,
                   public tchecker::ts::bwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>,
                   public tchecker::ts::fwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                                   tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                                   tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                                   tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>,
                   public tchecker::ts::bwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                                   tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                                   tchecker::zg::final_range_t, tchecker::zg::incoming_edges_range_t,
                                                   tchecker::zg::final_value_t, tchecker::zg::incoming_edges_value_t>,
                   public tchecker::ts::builder_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>,
                   public tchecker::ts::inspector_t<tchecker::zg::const_state_sptr_t, tchecker::zg::const_transition_sptr_t>,
                   public tchecker::ts::sharing_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t> {

public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                    tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>;
  using bwd_t = tchecker::ts::bwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                    tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                              tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                              tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>;
  using bwd_impl_t = tchecker::ts::bwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                              tchecker::zg::final_range_t, tchecker::zg::incoming_edges_range_t,
                                              tchecker::zg::final_value_t, tchecker::zg::incoming_edges_value_t>;
  using builder_t = tchecker::ts::builder_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>;
  using inspector_t = tchecker::ts::inspector_t<tchecker::zg::const_state_sptr_t, tchecker::zg::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>;


  using sst_t = fwd_t::sst_t;
  using state_t = fwd_t::state_t;
  using const_state_t = fwd_t::const_state_t;
  using transition_t = fwd_t::transition_t;
  using const_transition_t = fwd_t::const_transition_t;
  using initial_range_t = fwd_impl_t::initial_range_t;
  using initial_value_t = fwd_impl_t::initial_value_t;
  using outgoing_edges_range_t = fwd_impl_t::outgoing_edges_range_t;
  using outgoing_edges_value_t = fwd_impl_t::outgoing_edges_value_t;
  using final_range_t = bwd_impl_t::final_range_t;
  using final_value_t = bwd_impl_t::final_value_t;
  using incoming_edges_range_t = bwd_impl_t::incoming_edges_range_t;
  using incoming_edges_value_t = bwd_impl_t::incoming_edges_value_t;


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
  zg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
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
  zg_t(zg_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  zg_t(zg_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~zg_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  zg_t & operator=(zg_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  zg_t & operator=(zg_t &&) = delete;

  using fwd_t::state;
  using fwd_t::status;
  using fwd_t::transition;

  // Forward

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual initial_range_t initial_edges();

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
                       tchecker::state_status_t mask = tchecker::STATE_OK);

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
  virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual outgoing_edges_range_t outgoing_edges(tchecker::zg::const_state_sptr_t const & s);

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
  virtual void next(tchecker::zg::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);


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
  virtual void next(tchecker::zg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Backward

  /*!
   \brief Accessor
   \param labels : final states labels
   \return Final edges
   \note the return range goes across all tuples of process locations and bounded integer variables
   valuations in the given system. The number of such tuples is exponential in the number of locations
   and processes, as well as in the domains of bounded integer variables
   */
  virtual final_range_t final_edges(boost::dynamic_bitset<> const & labels);

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
                     tchecker::state_status_t mask = tchecker::STATE_OK);

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
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return incoming edges to state s
   \note the return range goes across all tuples of process locations and bounded integer variabkes
   valuations in the given system. The number of such tuples is exponential in the number of locations
   and processes, as well as the domains of bounded integer variables
   */
  virtual incoming_edges_range_t incoming_edges(tchecker::zg::const_state_sptr_t const & s);

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
  virtual void prev(tchecker::zg::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

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
  virtual void prev(tchecker::zg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

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
                     tchecker::state_status_t mask = tchecker::STATE_OK);

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
  void split(tchecker::zg::const_state_sptr_t const & s, tchecker::clock_constraint_t const & c,
             std::vector<tchecker::zg::state_sptr_t> & v);


  /*!
   \brief Split a state according to a list of clock constraints
   \param s : state
   \param constraints : clock constraints
   \param v : vector of states
   \post s has been successively split w.r.t. every constraint in constraints. All resulting states
   have been added to v
   */
  void split(tchecker::zg::const_state_sptr_t const & s, tchecker::clock_constraint_container_t const & constraints,
             std::vector<tchecker::zg::state_sptr_t> & v);


  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::zg::const_state_sptr_t const & s) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(tchecker::zg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(tchecker::zg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if s has a non-empty zone, false otherwise
  */
  virtual bool is_valid_final(tchecker::zg::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::zg::const_state_sptr_t const & s) const;

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::zg::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::zg::transition_sptr_t & t);

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
  inline tchecker::clock_id_t clocks_count()  { return _system->clocks_count(tchecker::VK_FLATTENED);  }

private:

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
                  tchecker::zg::const_state_sptr_t const & to_clone, bool clone
                  );
  /*!
   \brief Clone and constrain a state
   \param s : a state
   \param c : a clock constraint
   \return a clone of s with its zone intersected with c
   */
  tchecker::zg::state_sptr_t clone_and_constrain(tchecker::zg::const_state_sptr_t const & s,
                                                 tchecker::clock_constraint_t const & c);

  /*!
   \brief Clone and constrain a state
   \param s : a state
   \param c : a vector of clock constraints
   \return a clone of s with its zone intersected with c
   */
  tchecker::zg::state_sptr_t clone_and_constrain(tchecker::zg::const_state_sptr_t const & s,
                                                 clock_constraint_container_t const & c);

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
tchecker::zg::state_sptr_t
initial(tchecker::zg::zg_t& zg,
        tchecker::vloc_t const & vloc, tchecker::state_status_t mask = tchecker::STATE_OK);

/*!
 \brief Compute next state and transition from a tuple of edges
 \param zg : zone graph
 \param s : state
 \param vedge : tuple of edges
 \param mask : next state status mask
 \return the pair (nexts, nextt) of successor state and transition of s
 along tuple of edges vedge if any, (nullptr, nullptr) otherwise
 */
std::tuple<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>
next(tchecker::zg::zg_t &zg,
     tchecker::zg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge, tchecker::state_status_t mask = tchecker::STATE_OK);

/* factory */

std::shared_ptr<zg_t> factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             std::size_t table_size);

std::shared_ptr<zg_t> factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                             std::size_t table_size);

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_HH
