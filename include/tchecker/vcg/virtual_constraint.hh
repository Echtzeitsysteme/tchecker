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

  /*
  * accessor
  \return a reference to the clock constraints
  */
  const clock_constraint_container_t & get_vc() const;


private:
  const tchecker::clock_id_t _dim;
  const tchecker::clock_id_t _lowest_virtual_clk_index;
  const clock_constraint_container_t _vcs;

};

virtual_constraint_t * factory(const tchecker::zg::zone_t *zone, tchecker::clock_id_t no_of_virtual_clocks);

virtual_constraint_t * factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks);

} // end of namespace vcg

} // end of namespace tchecker

#endif
