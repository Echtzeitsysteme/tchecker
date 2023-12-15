/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VCG_VIRTUAL_CONSTRAINT_HH
#define TCHECKER_VCG_VIRTUAL_CONSTRAINT_HH

// forward declaration
namespace tchecker {
namespace zg {
class zone_t;
}
}

#include "tchecker/zg/zone.hh"
#include "tchecker/dbm/dbm.hh"

/*
 \file virtual_constraint.hh
 \brief our model of a virtual constraint
 */

namespace tchecker {

namespace vcg {

/*!
 \class virtual_constraint_t
 \brief Implementation of virtual constraints
 \note We model the virtual constraint as zone
 * using the virtual clocks, only. This is
 * fine since a zone always corresponds to a
 * clock constraint
 */

class virtual_constraint_t {

public:

  /*!
   \brief constructor
   \param zone : The zone from which we extract the virtual constraint
   \param no_of_virtual_clocks : The number of virtual clocks
   */
  virtual_constraint_t(const clock_constraint_container_t & vcs, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks);

  /*!
   \brief Copy constructor
   */
  virtual_constraint_t(tchecker::vcg::virtual_constraint_t const &) = default;

  /*
   * accessor
   \return a reference to the clock constraints
   */
  const clock_constraint_container_t & get_vc() const;

  /*
   * transforms the virtual constraints into a dbm.
   \return a tight DBM which contains the virtual clocks as a DBM. The DBM is allocated at the heap and needs to be freed.
   */
  tchecker::dbm::db_t * to_dbm() const;

  /*!
   \brief Accessor
   \return no of virtual clocks
   */
  const tchecker::clock_id_t get_no_of_virt_clocks() const;

private:
  // dimension of the zones used at the vcg
  const tchecker::clock_id_t _dim;

  // the number of virtual clocks
  const tchecker::clock_id_t _no_of_virtual_clocks;

  // the actual constrains
  const clock_constraint_container_t _vcs;

};

virtual_constraint_t * factory(const tchecker::zg::zone_t *zone, tchecker::clock_id_t no_of_virtual_clocks);

virtual_constraint_t * factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks);

} // end of namespace vcg

} // end of namespace tchecker

#endif
