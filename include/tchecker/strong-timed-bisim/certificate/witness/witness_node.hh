/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_NODE_HH
#define TCHECKER_STRONG_TIMED_BISIM_WITNESS_WITNESS_NODE_HH

#include "tchecker/strong-timed-bisim/certificate/certificate_node.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace witness {

 /*!
 \class node_t
 \brief Node of the witness graph of a strong timed bisimulation check
 */
class node_t : public tchecker::strong_timed_bisim::certificate::node_t {
 public:
  /*!
  \brief Constructor
  \param s_1 : a symbolic state of the first VCG
  \param s_2 : a symbolic state of the second VCG
  \param id : the id of this node within the witness graph
  \post this node keeps a shared pointer to s, and has initial node flag as specified
  */
  node_t(tchecker::zg::state_sptr_t const & s_1, tchecker::zg::state_sptr_t const & s_2, 
         tchecker::clock_id_t no_of_virt_clks, std::size_t id=0, bool initial = false);

  /*!
   \brief Constructor
   \param location_pair : the pair of locations to add
   \param vc : the virtual constraints of the synchronized symbolic states to add
   \param id : the id of this node within the witness graph
  */
  node_t(std::pair<tchecker::ta::state_t, tchecker::ta::state_t> & location_pair,
         std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vc, 
         std::size_t id=0, bool initial = false);

  /*!
   \brief Constructor
   \param first_loc : the first location
   \param second_loc : the second location
   \param vc : the virtual constraints of the synchronized symbolic states to add
   \param id : the id of this node within the witness graph
  */
  node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, 
         std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> vc, 
         std::size_t id=0, bool initial = false);

  /*!
   \brief Constructor
   \param first_loc : the first location
   \param second_loc : the second location
   \param no_of_virt_clks : the number of virtual clocks
   \param id : the id of this node within the witness graph
  */
  node_t(tchecker::ta::state_t &first_loc, tchecker::ta::state_t & second_loc, tchecker::clock_id_t no_of_virt_clks, 
         std::size_t id=0, bool initial = false);

   /*!
  \brief Accessor
  \return a pointer to zones
  */
  inline const std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>>
  zones() const { return _zones; }

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

  /*!
   \brief checks whether this node is fulfillable
   \return true if and only if there is no fulfillable zone in _zones.
   */
  bool empty() const;

  /*!
   \brief compresses the zones of this node
   */
  void compress();

  /*!
  \brief Accessor to node attributes
  \param m : the map to add the attributes
  \param vcg1: the first vcg
  \param vcg2 : the second vcg
  \post The attributes of this node are added to m
  */
  void attributes(std::map<std::string, std::string> & m, 
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg1,
                  const std::shared_ptr<tchecker::vcg::vcg_t> vcg2) const override;

private:
  std::shared_ptr<tchecker::zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>> _zones;
};

} // end of namespace witness

} // end of namespace strong_timed_bisim

} // end of namespace tchecker

#endif
