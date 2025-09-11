/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_NODE_HH
#define TCHECKER_STRONG_TIMED_BISIM_CERTIFICATE_CERTIFICATE_NODE_HH

#include "tchecker/graph/node.hh"
#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/zg/zone_container.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace certificate {

/*!
 \class node_t
 \brief Node of the certificates of a strong timed bisimulation check
 */
class node_t {
 public:
  /*!
  \brief Constructor
  \param s_1 : a symbolic state of the first VCG
  \param s_2 : a symbolic state of the second VCG
  \param id : the id of this node within the witness graph
  \param initial : whether this is the initial node of the graph
  \post this node keeps a shared pointer to s, and has initial node flag as specified
  */
  node_t(tchecker::zg::state_sptr_t const & s_1, tchecker::zg::state_sptr_t const & s_2, 
         std::size_t id = 0, bool initial = false);

  /*!
   \brief Constructor
   \param location_pair : the pair of locations to add
   \param id : the id of this node within the witness graph
   \param initial : whether this is the initial node of the graph
  */
  node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
         std::size_t id = 0, bool initial = false);

  /*!
   \brief Constructor
   \param first_loc : the first location
   \param second_loc : the second location
   \param id : the id of this node within the witness graph
   \param initial : whether this is the initial node of the graph
  */
  node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, 
         std::size_t id = 0, bool initial = false);

  /*!
   \brief Accessor
   \return shared pointer to the pair of locations
   */
  inline std::shared_ptr<const std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> location_pair_ptr() const { return _location_pair; }

  /*!
  \brief Accessor
  \return pair of locations
  */
  inline std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair() const { return *_location_pair; }

 /*!
  \brief Accessor
  \return id in this node
  */
  inline std::size_t id() const { return _id; }

  /*!
  \brief Setter
  \param id : new id of this node
  \post _id = id
  */
  void set_id(std::size_t id) { _id = id; }

   /*!
  \brief Accessor
  \return whether this node is initial
  */
  inline bool initial() const { return _initial; }

  /*!
  \brief Hash function
  \return hash value of this node
  */
  std::size_t hash() const;

  /*!
   \brief Less-than ordering on nodes based on lexical ordering
   \param other : an edge
   \return true if lhs is less-than rhs w.r.t. the tuple of edges in e1 and e2
   \note id and initial are not part of this comparison!
  */
  bool operator<(const node_t & other) const;

  /*!
   \brief equal operator
   \param other : an edge type
   \return true if lhs is less-than rhs w.r.t. the tuple of edges in e1 and e2
   \note id and initial are not part of this comparison!
  */
  bool operator==(const node_t & other) const;

  /*!
   \brief Accessor to node attributes
   \param m : the map to add the attributes
   \param vcg1: the first vcg
   \param vcg2 : the second vcg
   \post The attributes of this node are added to m
   */
  virtual void attributes(std::map<std::string, std::string> & m, 
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const;
  
 protected:
  const std::shared_ptr<std::pair<tchecker::ta::state_t, tchecker::ta::state_t>> _location_pair;
  std::size_t _id;
  const bool _initial;
};

} // end of namespace certificate

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif