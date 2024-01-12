/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VCG_REVERT_TRANSITIONS_HH
#define TCHECKER_VCG_REVERT_TRANSITIONS_HH

#include "tchecker/vcg/virtual_constraint.hh"
#include "tchecker/zg/zone.hh"

/*!
 \brief revert-action-trans function (see the TR of Lieb et al.)
 \param zone : the original zone
 \param guard : of the transition to revert
 \param reset : the reset set of the transition to revert
 \param tgt_invariant : the invariant of the target state of the transition to revert
 \param phi_split : the sub vc of the target
 \return a shared pointer to the resulting virtual constraint
 */
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
revert_action_trans(const tchecker::zg::zone_t & zone,
                    const tchecker::clock_constraint_container_t & guard,
                    const tchecker::clock_reset_container_t & reset,
                    const tchecker::clock_constraint_container_t & tgt_invariant,
                    const tchecker::virtual_constraint::virtual_constraint_t & phi_split);

/*!
 \brief revert-epsilon-trans function (see the TR of Lieb et al.)
 \param zone : the original zone
 \param phi_split : the sub vc of the target
 \return a shared pointer to the resulting virtual constraint
 */
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
revert_epsilon_trans(const tchecker::zg::zone_t & zone, const tchecker::virtual_constraint::virtual_constraint_t & phi_split);

#endif
