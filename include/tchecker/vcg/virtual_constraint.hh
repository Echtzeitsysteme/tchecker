/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VCG_VIRTUAL_CONSTRAINT_HH
#define TCHECKER_VCG_VIRTUAL_CONSTRAINT_HH

#include "tchecker/zg/zone.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/zg/zone_container.hh"
#include "tchecker/vcg/vcg.hh"

/*
 \file virtual_constraint.hh
 \brief our model of a virtual constraint
 */

namespace tchecker {

// forward declaration
template <typename T>
class zone_container_t;

namespace virtual_constraint {

/*!
 \class virtual_constraint_t
 \brief Implementation of virtual constraints
 \note We model the virtual constraint as zone
 * using the virtual clocks, only. Do not add
 * any fields to this class (or change the factory)
 */

class virtual_constraint_t : public tchecker::zg::zone_t {

public:

  /*!
   \brief delete all constructors, instantiate this class using a factory function
   */
  virtual_constraint_t() = delete;

  /*!
   \brief Destructor
   */
  ~virtual_constraint_t() = default;

  /*!
   \brief Accessor
   \return no of virtual clocks
   */
  tchecker::clock_id_t get_no_of_virtual_clocks() const;

  /*!
   \brief return the virtual constraint as list of clock constraints
   \param no_of_orig_clocks : the number of orig clocks, used as offset
   */
  clock_constraint_container_t get_vc(tchecker::clock_id_t no_of_orig_clocks, bool system_clocks) const;

  /*!
   \brief returns (not this and other)
   \param result : the pointer in which the result will be stored. Has to be allocated!
   \param other : the other vc not this shall be anded with
   */
  void neg_logic_and(std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result, const virtual_constraint_t & other) const;

 /*!
  \brief returns (this and zone)
  \param result : the pointer in which the result will be stored. Has to be allocated!
  \param zone : the zone this shall be anded with
  \return EMPTY if the resulting DBM is empty, NON_EMPTY otherwise
  */
  enum tchecker::dbm::status_t logic_and(std::shared_ptr<virtual_constraint_t> result, const virtual_constraint_t & other) const;

 /*!
  \brief returns (this and zone)
  \param result : the pointer in which the result will be stored. Has to be allocated!
  \param zone : the zone this shall be anded with
  \return EMPTY if the resulting DBM is empty, NON_EMPTY otherwise
  */
  enum tchecker::dbm::status_t logic_and(std::shared_ptr<tchecker::zg::zone_t> result, const tchecker::zg::zone_t & zone) const;

 /*!
  \brief returns (this and zone)
  \param result : the ref in which the result will be stored.
  \param zone : the zone this shall be anded with
  \return EMPTY if the resulting DBM is empty, NON_EMPTY otherwise
  */
  enum tchecker::dbm::status_t logic_and(tchecker::zg::zone_t & result, const tchecker::zg::zone_t & zone) const;

 /*!
  \brief iterates through the container and logically ands each element with this
  \param result : the pointer in which the result will be stored. Has to be allocated!
  \param container : the container to and with
  \return a container with each element of container being logically anded with this.
  */
  void logic_and(std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> result,
                 std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> const container) const;

 /*!
  \brief returns, whether there exists any clock valuation that fulfills this
  \return false, if no clock valuation exists that fulfills this
  */
  bool is_fulfillable() {return !this->is_empty();}

 /*!
  \brief generates the corresponding synchronized zones
  \param no_of_orig_clocks_first : the number of original clocks of the first TA
  \param no_of_orig_clocks_second : the number of original clocks of the second TA
  \return a pair of synchronized zones such that the first has no_of_orig_clocks_first + number of virtual clocks clocks and analog for the second
  */
  std::pair<std::shared_ptr<tchecker::zg::zone_t>, std::shared_ptr<tchecker::zg::zone_t>> 
  generate_synchronized_zones(tchecker::clock_id_t no_of_orig_clocks_first, 
                              tchecker::clock_id_t no_of_orig_clocks_second);

private:

  std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> neg_helper(tchecker::dbm::db_t *upper_bounds) const;

};

// factories
std::shared_ptr<virtual_constraint_t> factory(tchecker::clock_id_t no_of_virtual_clocks);

std::shared_ptr<virtual_constraint_t> factory(tchecker::virtual_constraint::virtual_constraint_t const & virtual_constraint);

/*!
 \brief extract the virtual constraint from a zone
 \param zone : the zone from which the virtual constraint should be extracted
 \param virtual_clocks : the number of virtual clocks
 \return the virtual constraint of zone
 */
std::shared_ptr<virtual_constraint_t> factory(std::shared_ptr<tchecker::zg::zone_t const> zone, tchecker::clock_id_t no_of_virtual_clocks);

/*!
 \brief extract the virtual constraint from a zone
 \param zone : the zone from which the virtual constraint should be extracted
 \param virtual_clocks : the number of virtual clocks
 \return the virtual constraint of zone
 */
std::shared_ptr<virtual_constraint_t> factory(tchecker::zg::zone_t const & zone, tchecker::clock_id_t no_of_virtual_clocks);

/*!
 \brief extract the virtual constraint from a dbm
 \param dbm : the dbm from which the virtual constraint should be extracted
 \param dim : dimension of the dbm
 \param virtual_clocks : the number of virtual clocks
 \return the virtual constraint of zone
 */
std::shared_ptr<virtual_constraint_t> factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks);

/*!
 \brief combine operator (see the TR of Lieb et al.)
 \param a container of virtual constraints
 \return a container of virtual constraints
 */
std::shared_ptr<tchecker::zone_container_t<virtual_constraint_t>> combine(tchecker::zone_container_t<virtual_constraint_t> & lo_vc, tchecker::clock_id_t no_of_virtual_clocks);

} // end of namespace virtual_constraint

} // end of namespace tchecker

#endif
