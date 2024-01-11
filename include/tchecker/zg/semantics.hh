/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_SEMANTICS_HH
#define TCHECKER_ZG_SEMANTICS_HH

#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file semantics.hh
 \brief Operational semantics on DBMs
 */

namespace tchecker {

namespace zg {

/*!
 \class semantics_t
 \brief Semantics for zone graphs implemented with DBMs
 */
class semantics_t {
public:
  /*!
  \brief Destructor
   */
  virtual ~semantics_t() = default;

  /*!
  \brief Compute initial zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in initial state
  \param invariant : invariant
  \post dbm is the initial zone w.r.t. delay_allowed and invariant
  \return STATE_OK if the resulting dbm is not empty, other values if the
  resulting dbm is empty (see details in implementations)
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant) = 0;

  /*!
  \brief Compute final zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in final state
  \param invariant : invariant
  \post dbm is the final (target) zone w.r.t. delay_allowed and invariant
  \return STATE_OK if the resulting dbm is not empty, other values if the
  resulting dbm is empty (see details in implementations)
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant) = 0;

  /*!
  \brief Compute next zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has been updated to its strongest postcondition w.r.t. src_delay_allowed,
  src_invariant, guard, clkreset, tgt_delay_allowed and tgt_invariant
  \return STATE_OK if the resulting dbm is not empty, other values if the resulting
  dbm is empty (see details in implementations)
   */
  virtual tchecker::state_status_t next(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant) = 0;

  /*!
  \brief Compute previous zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has been updated to its weakest precondition w.r.t. src_delay_allowed,
  src_invariant, guard, clkreset, tgt_delay_allowed and tgt_invariant
  \return STATE_OK if the resulting dbm is not empty, other values if the resulting
  dbm is empty (see details in implementations)
   */
  virtual tchecker::state_status_t prev(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant) = 0;
};

/*!
\class standard_semantics_t
\brief Standard semantics: each transition in the zone graph consists of a delay
(if allowed) followed by a transition from the timed automaton
*/
class standard_semantics_t final : public tchecker::zg::semantics_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~standard_semantics_t() = default;

  /*!
  \brief Compute initial zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in initial state
  \param invariant : invariant
  \post dbm is the zone that only containts the zero valuation
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zero valuation does not
  satisfy invariant.
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute final zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in final state
  \param invariant : invariant
  \post dbm is the universal positive zone restricted to invariant
  \return STATE_OK if the resulting dbm is not empty. Otherwise,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the intersection of the
  universal positive zone and invariant is empty
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute next zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has been delayed (if allowed), then intersected with src_invariant,
  then intersected with guard, then reset w.r.t clkreset, then intersected with
  tgt_invariant
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone, tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection
  with guard result in an empty zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone
  */
  virtual tchecker::state_status_t next(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant);

  /*!
  \brief Compute previous zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has been intersected with target invariant and with resets, then reset
  clocks have been freed, then the resulting zone has been intersected with guard and
  source invariant, then opened down if delay is allowed and intersected with source
  invariant again
  \return STATE_OK if the resulting dbm is not empty,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED is intersection with tgt_invariant
  resulted in empty zone,
  tchecker::STATE_CLOCKS_RESET_FAILED if intersection with clkreset (as constraints)
  resulted in empty zone
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard resulted in
  empty zone
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  resulted in empty zone
   */
  virtual tchecker::state_status_t prev(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant);
};

/*!
\class elapsed_semantics_t
\brief elapsed semantics: each transition in the zone graph consists of a
transition from the automaton, followed by a delay (if allowed)
*/
class elapsed_semantics_t final : public tchecker::zg::semantics_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~elapsed_semantics_t() = default;

  /*!
  \brief Compute initial zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in initial state
  \param invariant : invariant
  \post dbm is the zone that contains all the time successors of the zero
  valuation that satisfy invariant if delay_allowed, otherwise the zero
  valuation if it satisfies invariant
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS__INVARIANT_VIOLATED if the (time successors of the)
  zero valuation does not satisfy invariant.
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute final zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in final state
  \param invariant : invariant
  \post dbm is the universal positive zone restricted to invariant
  \return STATE_OK if the resulting dbm is not empty. Otherwise,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the intersection of the
  universal positive zone and invariant is empty
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute next zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has been intersected with src_invariant, then intersected with
  guard, then reset w.r.t clkreset, then intersected with tgt_invariant, then
  delayed (if allowed) and intersected wth tgt_invariant again (if delayed)
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone, tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection
  with guard result in an empty zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone
  */
  virtual tchecker::state_status_t next(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant);

  /*!
  \brief Compute previous zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has first been intersected with target invariant. Then, if tgt_delay_allowed,
  the dbm has been opened down and intersected again with target invariant. It has then
  been intersected with clock resets. Then reset clocks have been freed. The resulting zone
  is then intersected with guard and src_invariant
  \return STATE_OK if the resulting dbm is not empty,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED is intersection with tgt_invariant
  resulted in empty zone,
  tchecker::STATE_CLOCKS_RESET_FAILED if intersection with clkreset (as constraints)
  resulted in empty zone
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard resulted in
  empty zone
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  resulted in empty zone
   */
  virtual tchecker::state_status_t prev(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant);
};

/*!
\class distinguished_semantics_t
\brief Distinguished semantics: "classic" semantics. A transition is either
* an action transition or a delay transition.
*/
class distinguished_semantics_t final : public tchecker::zg::semantics_t {
public:

  /*!
  \brief Destructor
  */
  virtual ~distinguished_semantics_t() = default;

  /*!
  \brief Compute initial zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : ignored
  \param invariant : invariant
  \post dbm is the zone that only contains the zero valuation
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zero valuation does not
  satisfy invariant.
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute final zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param delay_allowed : true if delay is allowed in final state
  \param invariant : invariant
  \post dbm is the universal positive zone restricted to invariant
  \return STATE_OK if the resulting dbm is not empty. Otherwise,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the intersection of the
  universal positive zone and invariant is empty
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute next zone when epsilon transition is used
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param invariant : invariant of current state
  \post dbm has been delayed (if allowed), then intersected with src_invariant,
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone
  */
  tchecker::state_status_t delay( tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                  tchecker::clock_constraint_container_t const & invariant);

  /*!
  \brief Compute next zone when action transition is used
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : ignored (always false in this semantics)
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : ignored (always false in this semantics)
  \param tgt_invariant : invariant in target state
  \post dbm has been intersected with src_invariant, then intersected with guard, then 
  reset w.r.t clkreset, then intersected with tgt_invariant
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone, tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection
  with guard result in an empty zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone
  */
  virtual tchecker::state_status_t next(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant);

  /*!
  \brief Compute previous zone
  \param dbm : a DBM
  \param dim : dimension of dbm
  \param src_delay_allowed : true if delay allowed in source state
  \param src_invariant : invariant in source state
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : true if delay allowed in target state
  \param tgt_invariant : invariant in target state
  \post dbm has been intersected with target invariant and with resets, then reset
  clocks have been freed, then the resulting zone has been intersected with guard and
  source invariant, then opened down if delay is allowed and intersected with source
  invariant again
  \return STATE_OK if the resulting dbm is not empty,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED is intersection with tgt_invariant
  resulted in empty zone,
  tchecker::STATE_CLOCKS_RESET_FAILED if intersection with clkreset (as constraints)
  resulted in empty zone
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard resulted in
  empty zone
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  resulted in empty zone
   */
  virtual tchecker::state_status_t prev(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, bool src_delay_allowed,
                                        tchecker::clock_constraint_container_t const & src_invariant,
                                        tchecker::clock_constraint_container_t const & guard,
                                        tchecker::clock_reset_container_t const & clkreset, bool tgt_delay_allowed,
                                        tchecker::clock_constraint_container_t const & tgt_invariant);

};


/*!
 \brief type of semantics
*/
enum semantics_type_t {
  STANDARD_SEMANTICS, /*!< see tchecker::zg::standard_semantics_t */
  ELAPSED_SEMANTICS,  /*!< see tchecker::zg::elapsed_semantics_t */
  DISTINGUISHED_SEMANTICS, /*!< see tchecker::zg::distinguished_semantics_t */
};

/*!
 \brief Zone graph semantics factory
 \param semantics_type : type zone graph semantics
 \return A zone graph semantics of type semantics
 \note the returned semantics must be deallocated by the caller
 \throw std::invalid_argument : if semantics is unknown
*/
tchecker::zg::semantics_t * semantics_factory(enum semantics_type_t semantics_type);

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_SEMANTICS_HH
