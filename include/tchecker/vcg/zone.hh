/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VCG_ZONE_HH
#define TCHECKER_VCG_ZONE_HH

#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/zg/zone.hh"

namespace tchecker {

namespace vcg {

/*!
 \file zone.hh
 \brief DBM implementation of zones for vcgs
 */
class zone_t : public tchecker::zg::zone_t {

public:
 /*!
  \brief revert-action-trans function (see the TR of Lieb et al.)
  \param guard : of the transition to revert
  \param reset : the reset set of the transition to revert
  \param tgt_invariant : the invariant of the target state of the transition to revert
  \param phi_split : the sub vc of the target
  \return a shared pointer to the resulting virtual constraint
  */
  std::shared_ptr<virtual_constraint_t> revert_action_trans(const tchecker::clock_constraint_container_t & guard,
                                                                 const tchecker::clock_reset_container_t & reset,
                                                                 const tchecker::clock_constraint_container_t & tgt_invariant,
                                                                 const virtual_constraint_t & phi_split);

  /*!
  \brief revert-epsilon-trans function (see the TR of Lieb et al.)
  \param zone : the original zone
  \param phi_split : the sub vc of the target
  \return a shared pointer to the resulting virtual constraint
  */
  std::shared_ptr<virtual_constraint_t> revert_epsilon_trans(const virtual_constraint_t & phi_split);

protected:
  /*!
   \brief Constructor
   \param dim : dimension
   \post this zone has dimension dim and is the universal zone
   */
  zone_t(tchecker::clock_id_t dim);

  /*!
   \brief Copy constructor
   \param zone : a zone
   \pre this has been allocated with the same dimension as zone
   \post this is a copy of zone
   */
  zone_t(tchecker::zg::zone_t const & zone);

  /*!
   \brief Move constructor
   \note deleted (move construction is the same as copy construction)
   */
  zone_t(tchecker::zg::zone_t && zone) = delete;

  /*!
   \brief Destructor
   */
  ~zone_t();

};

/*!
 \brief factory of zones of a vcg
 \param dim : the dimension
 */
zone_t * factory(tchecker::clock_id_t dim);

/*!
 \brief factory of zones of a vcg
 \param dim : the dimension
 \param zone : the zone to copy
 */

zone_t * factory(tchecker::clock_id_t dim, zone_t const & zone);

} // end of namespace vcg

} // end of namespace tchecker

#endif
