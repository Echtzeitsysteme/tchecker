/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_NODE_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_NODE_HH

#include "tchecker/graph/node.hh"
#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/zg/zone_container.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

 /*!
 \class node_t
 \brief Node of the witness graph of a strong timed bisimulation check
 */
class node_t {
 public:
  /*!
  \brief Constructor
  \param s_1 : a symbolic state of the first VCG
  \param s_2 : a symbolic state of the second VCG
  \param id : the id of this node within the witness graph
  \post this node keeps a shared pointer to s, and has initial node flag as specified
  */
  node_t(tchecker::zg::state_sptr_t const & s_1, tchecker::zg::state_sptr_t const & s_2, std::size_t id, tchecker::clock_id_t no_of_virt_clks, bool initial);

  /*!
   \brief Constructor
   \param location_pair : the pair of locations to add
   \param vc : the virtual constraints of the synchronized symbolic states to add
   \param id : the id of this node within the witness graph
  */
  node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair, tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & vc, std::size_t id);

  /*!
   \brief Constructor
   \param first_loc : the first location
   \param second_loc : the second location
   \param vc : the virtual constraints of the synchronized symbolic states to add
   \param id : the id of this node within the witness graph
  */
  node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> & vc, std::size_t id);

  /*!
   \brief Constructor
   \param first_loc : the first location
   \param second_loc : the second location
   \param no_of_virt_clks : the number of virtual clocks
   \param id : the id of this node within the witness graph
  */
  node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, 
         tchecker::clock_id_t no_of_virt_clks, std::size_t id);

  /*!
  \brief Accessor
  \return shared pointer to zone graph state in this node
  */
  inline std::shared_ptr<const std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> location_pair_ptr() const { return _location_pair; }

  /*!
  \brief Accessor
  \return zone graph state in this node
  */
  inline const std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair() const { return *_location_pair; }

 /*!
  \brief Accessor
  \return id in this node
  */
  inline std::size_t id() const { return _id; }

   /*!
  \brief Accessor
  \return whether this node is initial
  */
  inline bool initial() const { return _initial; }

 /*!
   \brief Less-than order on nodes based on lexical ordering
   \param other : a node
   \return true if lhs is less-than rhs w.r.t. lexical ordering over the location pairs in
   the nodes
  */
 bool smaller_location_pair(const node_t& other) const;

 /*!
  \brief Equality predicate
  \param other : a node
  \return true if lhs and rhs have the same location pair, false otherwise
  */
  bool equal_location_pair(const node_t& other) const;

 /*!
  \brief Hash function
  \return hash value of this node
  */
  std::size_t hash() const;

 /*!
  \brief adds a condition to _zones
  \post vc is added to _zones
  */ 
  void add_zone(std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> vc);

  /*!
  \brief adds many conditions to _zones
  \post vc is added to _zones
  */ 
  void add_zones(tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> &vcs);

 private:
  const std::shared_ptr<const std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> _location_pair;
  tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t> _zones;
  const std::size_t _id;
  const bool _initial;
};

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
