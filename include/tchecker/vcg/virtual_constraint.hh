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
#include "tchecker/utils/zone_container.hh"

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
 * using the virtual clocks, only. This is
 * fine since a zone always corresponds to a
 * clock constraint
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
  const tchecker::clock_id_t get_no_of_virt_clocks() const;

  /*!
   \brief return the virtual constraint as list of clock constraints
   \param sum_of_orig_clocks : the sum of the original clocks of both TA
   */
  clock_constraint_container_t get_vc(tchecker::clock_id_t orig_clocks_offset) const;

  /*!
   \brief returns the negated version of this clock constraint
   \return let result be the return value.
   *  forall u with u model this and for all vc in result u does not model vc
   *  forall u with u does not model this exists a vc in result such that u models vc
   *  (for u in ({\chi_0, ..., \chi_{|C_A| + | C_B| - 1}} \rightarrow T))
   */
  tchecker::zone_container_t<virtual_constraint_t> * neg() const;

 /*
  \brief iterates through the container and logically ands each element with this
  \param container : the container to and with
  \return a copy of container with each element being logically anded with this.
  */
  tchecker::zone_container_t<virtual_constraint_t> logic_and(tchecker::zone_container_t<virtual_constraint_t> *container);

};

// factories
virtual_constraint_t * factory(tchecker::clock_id_t dim);

virtual_constraint_t * factory(tchecker::virtual_constraint::virtual_constraint_t const & virtual_constraint);

/*!
 \brief extract the virtual constraint from a zone
 \param zone : the zone from which the virtual constraint should be extracted
 \param virtual_clocks : the number of virtual clocks
 \return the virtual constraint of zone
 */
virtual_constraint_t * factory(const tchecker::zg::zone_t *zone, tchecker::clock_id_t no_of_virtual_clocks);

/*!
 \brief extract the virtual constraint from a dbm
 \param dbm : the dbm from which the virtual constraint should be extracted
 \param dim : dimension of the dbm
 \param virtual_clocks : the number of virtual clocks
 \return the virtual constraint of zone
 */
virtual_constraint_t * factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks);

// destruction

void destruct(virtual_constraint_t *to_destruct);

/*!
 \brief combine operator (see the TR of Lieb et al.)
 \param a vector of vector of virtual constraints
 \return a vector of virtual constraints
 */
tchecker::zone_container_t<virtual_constraint_t> * combine(std::vector<tchecker::zone_container_t<virtual_constraint_t>> & lo_lo_vc, tchecker::clock_id_t dim);

} // end of namespace virtual_constraint

} // end of namespace tchecker

#endif
