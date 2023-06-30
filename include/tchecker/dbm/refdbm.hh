/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFDBM_HH
#define TCHECKER_REFDBM_HH

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file refdbm.hh
 \brief Functions on DBMs with reference clocks
 \note A DBM with reference clocks is a DBM where the first refcount variables
 are reference variables, and the other variables are clock (or offset)
 variables. Each clock variable has a corresponding reference variable. The
 correspondance is defined according to an instance r of
 tchecker::reference_clock_variables_t. It associates to each clock variable X
 its reference clock r(X). Each reference clock is mapped to itself.

 The value of system clock x is represented as the value of X-r(X), the
 difference between the corresponding clock variable X and its reference clock
 r(X). Reference DBMs generalise standard DBMs which have a single reference
 clock, usually denoted 0.

 Reference DBMs have been introduced as "offset DBMs" in "Partial Order
 Reduction for Timed Systems", J. Bengtsson, B. Jonsson, J. Lilis and Wang Yi,
 CONCUR, 1998.
 */

namespace tchecker {

namespace refdbm {

/*!
 \brief Universal DBM with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is a DBM over reference clocks r
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 \post dbm is the universal DBM over reference clocks r
 rdbm is tight and consistent.
 */
void universal(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Positive universal DBM with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 \post rdbm is the universal positive DBM over reference clocks r
 dbm is tight and consistent
 */
void universal_positive(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Empty DBM with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 \post rdbm represents the empty DBM over reference clocks r (is_empty_0() returns
 true on dbm)
 rdbm IS NOT TIGHT (empty DBMs cannot be tight), NOR CONSISTENT
 */
void empty(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Initialize DBM with reference clocks to zero
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 \post rdbm represents the zone where all variables are equal to each other over
 reference variables r.
 dbm is tight and consistent.
 */
void zero(tchecker::dbm::db_t * dbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Fast emptiness predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is either non-empty and tight, or it is empty
 rdbm is a DBM over reference clocks r
 \return true if rdbm has a negative difference bound on (0,0), false otherwise
 \note this function only checks the (0,0) entry of rdbm. Hence, this function
 returns false for empty DBMs which have no negative bound on (0,0).
 However, all other functions on DBMs with reference clocks set (0,0) to a value
 less-than <=0 when they generate an empty zone. So this function can be used as
 a safe and efficient emptiness check if rdbm has been generated by calls to
 functions over DBMs with reference variables defined in this file.
 */
bool is_empty_0(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Universality predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \return true if rdbm is universal, false otherwise
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 */
bool is_universal(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Positivity predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \return true if all clocks in rdbm have a value greater-than or equal-to their
 reference clock, false otherwise
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 */
bool is_positive(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Positive universality predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \return true if rdbm is universal positive (the only constraints in rdbm are
 positivity constraints: each clock is greater-than or equal-to its reference
 clock), false otherwise.
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 */
bool is_universal_positive(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Checks if a DBM with reference clocks is open up (time-elapsed)
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \return true if rdbm is open up (it is closed under time successors), false
 otherwise
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is consistent (checked by assertion)
 rdbm is a DBM over reference clocks r
 */
bool is_open_up(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Tightness predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks fr rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is a DBM over reference clocks r
 \return true if dbm is tight, false otherwise
 */
bool is_tight(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Tighten a  DBM with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is a DBM over reference clocks r
 rdbm is consistent (checked by assertion)
 \post rdbm is tight if it is not empty.
 if rdbm is empty, then the difference bound in (0,0) is less-than <=0
 (tchecker::refdbm::is_empty_0() returns true)
 \return EMPTY if rdbm is empty, NON_EMPTY otherwise
 \note Applies Floyd-Warshall algorithm on rdbm seen as a weighted graph.
 */
enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Consistency of DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is a DBM over reference clocks r
 \return true if rdbm is consistent (no negative value on the diagonal), false
 otherwise
*/
bool is_consistent(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Synchronized predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 \return true if reference clocks are equal to each other in rdbm, false otherwise
 */
bool is_synchronized(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Synchronized predicate on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param sync_ref_clocks : synchronized reference clocks
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 the size of sync_ref_clocks is the number of reference clocks in r (checked by
 assertion)
 \return true if all reference clocks in sync_ref_clocks are equal in rdbm,
 false otherwise
 */
bool is_synchronized(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r,
                     boost::dynamic_bitset<> const & sync_ref_clocks);

/*!
 \brief Check if a DBM with reference clocks contains a synchronized valuation
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 \return true dbm contains a synchronized valuation, false otherwise
 */
bool is_synchronizable(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Equality predicate on DBMs with reference clocks
 \param rdbm1 : a first DBM
 \param rdbm2 : a second DBM
 \param r : reference clocks for rdbm1 and rdbm2
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 rdbm1 and rdbm2 are DBMs over reference clocks r
 \return true if rdbm1 and rdbm2 are equal, false otherwise
 */
bool is_equal(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
              tchecker::reference_clock_variables_t const & r);

/*!
 \brief Inclusion predicate on DBMs with reference clocks
 \param rdbm1 : a first DBM
 \param rdbm2 : a second DBM
 \param r : reference clocks for rdbm1 and rdbm2
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 rdbm1 and rdbm2 are DBMs over reference clocks r
 \return true if rdbm1 is included into rdbm2, false otherwise
 */
bool is_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
           tchecker::reference_clock_variables_t const & r);

/*!
 \brief Checks inclusion w.r.t. abstraction aLU* (i.e. aLU over reference DBMs)
 \param rdbm1 : a first dbm
 \param rdbm2 : a second dbm
 \param r : reference clocks for rdbm1 and rdbm2
 \param l : clock lower bounds for offset clocks, l[0] is the bound for offset
 clock 1 and so on
 \param u : clock upper bounds for offset clocks, u[0] is the bound for offset
 clock 1 and so on
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are positive (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 l and u are arrays of size r.size()-r.refcount()
 l[i], u[i] < tchecker::dbm::INF_VALUE for all offset clock i>=0 (checked by
 assertion)
 \return true if rdbm1 <= aLU*(rdbm2), false otherwise
 \note set l[i]/u[i] to tchecker::clockbounds::NO_BOUND if clock i has no
 lower/upper bound
 */
bool is_alu_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                    tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l,
                    tchecker::integer_t const * u);

/*!
 \brief Checks inclusion w.r.t. abstraction aM* (i.e. aM over reference DBMs)
 \param rdbm1 : a first dbm
 \param rdbm2 : a second dbm
 \param r : reference clocks for rdbm1 and rdbm2
 \param m : clock bounds for offset clocks, m[0] is the bound for offset clock 1
 and so on
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are positive (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 m is an array of size r.size()-r.refcount()
 m[i] < tchecker::dbm::INF_VALUE for all offset clock i>=0 (checked by
 assertion)
 \return true if rdbm1 <= aM*(rdbm2), false otherwise
 \note set m[i] to tchecker::clockbounds::NO_BOUND if clock i has no lower/upper
 bound
 */
bool is_am_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                   tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m);

/*!
 \brief Checks inclusion w.r.t. abstraction aLU* (i.e. aLU over reference DBMs)
 combined with time elapse
 \param rdbm1 : a first dbm
 \param rdbm2 : a second dbm
 \param r : reference clocks for rdbm1 and rdbm2
 \param l : clock lower bounds for offset clocks, l[0] is the bound for offset
 clock 1 and so on
 \param u : clock upper bounds for offset clocks, u[0] is the bound for offset
 clock 1 and so on
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are positive (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 l and u are arrays of size r.size()-r.refcount()
 l[i], u[i] < tchecker::dbm::INF_VALUE for all offset clock i>=0 (checked by
 assertion)
 \return true if time-elapse(rdbm1) <= aLU*(time-elapse(rdbm2)), false otherwise
 \note set l[i]/u[i] to tchecker::clockbounds::NO_BOUND if clock i has no
 lower/upper bound
 \note time-elapsed(dbm1) and time-elapsed(dbm2) are not computed but the
 algorithm for aLU* is adapted to behave as if rdbm1 and rdbm2 were time-elapsed
 */
bool is_time_elapse_alu_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                                tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l,
                                tchecker::integer_t const * u);

/*!
 \brief Checks inclusion w.r.t. abstraction aM* (i.e. aM over reference DBMs)
 combined with time elapse
 \param rdbm1 : a first dbm
 \param rdbm2 : a second dbm
 \param r : reference clocks for rdbm1 and rdbm2
 \param m : clock bounds for offset clocks, m[0] is the bound for offset clock 1
 and so on
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are positive (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 m is an array of size r.size()-r.refcount()
 m[i] < tchecker::dbm::INF_VALUE for all offset clock i>=0 (checked by
 assertion)
 \return true if time-elapse(rdbm1) <= aM*(time-elapse(rdbm2)), false otherwise
 \note set m[i] to tchecker::clockbounds::NO_BOUND if clock i has no lower/upper
 bound
 \note time-elapsed(dbm1) and time-elapsed(dbm2) are not computed but the
 algorithm for aLU* is adapted to behave as if rdbm1 and rdbm2 were time-elapsed
 */
bool is_time_elapse_am_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                               tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m);

/*!
 \brief Checks inclusion w.r.t. abstraction aLU over synchronized valuations
 \param rdbm1 : a first dbm
 \param rdbm2 : a second dbm
 \param r : reference clocks for rdbm1 and rdbm2
 \param l : clock lower bounds for offset clocks, l[0] is the bound for offset
 clock 1 and so on
 \param u : clock upper bounds for offset clocks, u[0] is the bound for offset
 clock 1 and so on
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are positive (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 l and u are arrays of size r.size()-r.refcount()
 l[i], u[i] < tchecker::dbm::INF_VALUE for all offset clock i>=0 (checked by
 assertion)
 \return true if sync(local_time_elapse(rdbm1)) <=
 aLU(sync(local_time_elapse(rdbm2))), false otherwise
 \note set l[i]/u[i] to tchecker::clockbounds::NO_BOUND if clock i has no
 lower/upper bound
 */
bool is_sync_alu_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                    tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l,
                    tchecker::integer_t const * u);

/*!
 \brief Checks inclusion w.r.t. abstraction aM over synchronized valuations
 \param rdbm1 : a first dbm
 \param rdbm2 : a second dbm
 \param r : reference clocks for rdbm1 and rdbm2
 \param m : clock bounds for offset clocks, m[0] is the bound for offset clock 1
 and so on
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 and rdbm2 are r.size()*r.size() arrays of difference bounds
 rdbm1 and rdbm2 are consistent (checked by assertion)
 rdbm1 and rdbm2 are positive (checked by assertion)
 rdbm1 and rdbm2 are tight (checked by assertion)
 m is an array of size r.size()-r.refcount()
 m[i] < tchecker::dbm::INF_VALUE for all offset clock i>=0 (checked by
 assertion)
 \return true if sync(rdbm1) <= aM(sync(rdbm2)), false otherwise
 \note set m[i] to tchecker::clockbounds::NO_BOUND if clock i has no bound
 */
bool is_sync_am_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                   tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m);

/*!
 \brief Hash function on DBMs with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \return hash value for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is a DBM over reference clocks r
 \note if rdbm is not tight, the returned hash code may differ from the hash
 code of its corresponding tight DBM
 */
std::size_t hash(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Constrain a DBM with reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param x : index of first clock
 \param y : index of second clock
 \param cmp : constraint comparator
 \param value : constraint value
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 0 <= x < r.size() (checked by assertion)
 0 <= y < r.size() (checked by assertion)
 \post rdbm has been intersected with constraint `x - y # value` where # is < if
 cmp is LT, and # is <= if cmp is LE
 rdbm is tight if it is not empty.
 if rdbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::refdbm::is_empty_0() returns true)
 \return EMPTY if rdbm is empty, NON_EMPTY otherwise
 \throw std::invalid_argument : if `cmp value` cannot be represented as a
 tchecker::dbm::db_t (only if compilation flag DBM_UNSAFE is not set)
 */
enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                       tchecker::clock_id_t x, tchecker::clock_id_t y, tchecker::ineq_cmp_t cmp,
                                       tchecker::integer_t value);

/*!
 \brief Constrain a DBM with reference clocks with a clock constraint
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param c : clock constraint
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 clocks IDs in c are systems clocks, i.e. the first clock has index 0, and the
 last clock has index r.size() - r.refcount() - 1
 \post rdbm has been intersected with constraint c (translated w.r.t. r clocks)
 rdbm is tight if it is not empty.
 if rdbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::refdbm::is_empty_0() returns true)
 \return EMPTY if rdbm is empty, NON_EMPTY otherwise
 */
enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                       tchecker::clock_constraint_t const & c);
/*!
 \brief Constrain a DBM with reference clocks with a collection of clock constraints
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param cc : collection of clock constraints
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 clocks IDs in c are systems clocks, i.e. the first clock has index 0, and the
 last clock has index r.size() - r.refcount() - 1
 \post rdbm has been intersected with clock constraints in cc (translated w.r.t.
 r clocks))
 rdbm is tight if it is not empty.
 if rdbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::refdbm::is_empty_0() returns true)
 \return EMPTY if rdbm is empty, NON_EMPTY otherwise
 */
enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                       tchecker::clock_constraint_container_t const & cc);

/*!
 \brief Restriction to synchronized valuations
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 \post rdbm has been restricted to its subset of synchronized valuations
 \return tchecker::dbm::EMPTY if synchronized dbm is empty,
 tchecker::dbm::NON_EMPTY otherwise
 */
enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Restriction to synchronized valuations over specified set of reference
 clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param sync_ref_clocks : set of reference clocks to synchronize
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 the size of sync_ref_clocks is the number of reference clocks in r (checked by
 assertion)
 \post rdbm has been restricted to its subset of valuations that are
 synchronized over all reference clocks in sync_ref_clocks
 \return tchecker::dbm::EMPTY if synchronized dbm is empty,
 tchecker::dbm::NON_EMPTY otherwise
 */
enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         boost::dynamic_bitset<> const & sync_ref_clocks);

/*!
 \brief Unbounded spread
*/
extern tchecker::integer_t const UNBOUNDED_SPREAD;

/*!
 \brief Bound the spread between reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param spread : expected spread between reference clocks
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 \post rdbm is unchanged if spread is tchecker::refdbm::UNBOUNDED_SPREAD.
 rdbm has been restricted to its subset of valuations such that the spread
 between all reference clocks is bounded by spread otherwise, if not empty.
 if rdbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::refdbm::is_empty_0() returns true)
 \return tchecker::dbm::EMPTY is the spread-bounded dbm is empty,
 tchecker::dbm::NON_EMPTY otherwise
 */
enum tchecker::dbm::status_t bound_spread(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                          tchecker::integer_t spread);

/*!
 \brief Bound the spread between reference clocks
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param spread : expected spread between reference clocks
 \param ref_clocks : reference clocks to bound
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 the size of ref_clocks is the number of reference clocks in r (checked by
 assertion)
 \post rdbm is unchanged if spread is tchecker::refdbm::UNBOUNDED_SPREAD.
 rdbm has been restricted to its subset of valuations such that the spread
 between all reference clocks in ref_clocks is bounded by spread otherwise, if
 not empty.
 if rdbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::refdbm::is_empty_0() returns true)
 \return tchecker::dbm::EMPTY is the spread-bounded dbm is empty,
 tchecker::dbm::NON_EMPTY otherwise
 */
enum tchecker::dbm::status_t bound_spread(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                          tchecker::integer_t spread, boost::dynamic_bitset<> const & ref_clocks);

/*!
 \brief Reset a clock to its reference clock
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param x : clock identifier
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 x < r.size() (checked by assertion)
 \post clock x has been updated to value r.refmap()[x] in dbm. All other
 variables are unchanged
 rdbm is tight
 */
void reset_to_reference_clock(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                              tchecker::clock_id_t x);

/*!
 \brief Reset a DBM with reference clocks w.r.t. a clock reset
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param reset : clock reset
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 all clock IDs in reset are system clock IDs
 reset should be a reset to reference clock: the right id of reset is
 tchecker::REFCLOCK_ID, and the value in reset is 0 (checked by assertion)
 \post the left clock in reset is equal to its reference clock. All other
 variables are unchanged.
 rdbm is tight
 */
void reset(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r, tchecker::clock_reset_t const & reset);

/*!
 \brief Reset a DBM with reference clocks w.r.t. a clock reset
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param rc : clock reset collection
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 all clock IDs in rc are system clock IDs
 all resets in rc must be resets to reference clock: the right id is
 tchecker::REFCLOCK_ID, and the value  is 0 (checked by assertion)
 \post the left clock in each reset in cr is equal to its reference clock. All
 other variables are unchanged.
 rdbm is tight
 */
void reset(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
           tchecker::clock_reset_container_t const & rc);

/*!
 \brief Asynchronous open-up (delay)
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 \post upper bounds on reference clocks and constraints relating reference
 clocks have been removed from rdbm (i.e. they have been set to (<,inf)).
 rdbm is tight and consistent.
 */
void asynchronous_open_up(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Asynchronous open-up (delay) with clock stopping
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \param delay_allowed : reference clocks allowed to delay
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 rdbm is a DBM over reference clocks r
 delay_allowed has size r.refcount() (checked by assertion)
 \post reference clocks in rdbm with delay_allowed are unbounded (i.e. x-t<inf
 for every reference clock t and any variable x, including x being another
 reference clock).
 reference clocks in rdbm without delay_allowed are unchanged
 rdbm is tight and consistent.
 */
void asynchronous_open_up(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                          boost::dynamic_bitset<> const & delay_allowed);

/*!
 \brief Extract a standard DBM from a DBM with reference clocks
 \param rdbm : a DBM with reference clocks
 \param r : reference clocks for rdbm
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is an r.size()*r.size() array of difference bounds (checked by assertion)
 rdbm is not empty.
 rdbm is consistent (checked by assertion)
 rdbm is tight (checked by assertion)
 rdbm is synchronized (checked by assertion)
 rdbm is a DBM over reference clocks r
 dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim is equal to r.size() - r.refcount() + 1 (checked by assertion)
 \post dbm is the zone extracted from rdbm by identifying the reference clocks
 in rdbm to the zero clock in dbm.
 dbm is tight and consistent.
 */
void to_dbm(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r, tchecker::dbm::db_t * dbm,
            tchecker::clock_id_t dim);

/*!
 \brief Output a DBM with reference clocks as a matrix
 \param os : output stream
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is a DBM over reference clocks r
 \post rdbm has been output to os as a matrix
 \return os after output
 */
std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * rdbm,
                             tchecker::reference_clock_variables_t const & r);

/*!
 \brief Output a DBM with reference clocks as a conjunction of constraints
 \param os : output stream
 \param rdbm : a DBM
 \param r : reference clocks for rdbm
 \pre rdbm is not nullptr (checked by assertion)
 rdbm is a r.size()*r.size() array of difference bounds
 rdbm is a DBM over reference clocks r
 \post the relevant constraints in rdbm have been output to os. Relevant
 constraints are those that differ from the universal DBM.
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r);

/*!
 \brief Lexical ordering over DBMs with reference clocks
 \param rdbm1 : first DBM
 \param r1 : reference clocks for rdbm1
 \param rdbm2 : second DBM
 \param r2 : reference clocks for rdbm2
 \pre rdbm1 and rdbm2 are not nullptr (checked by assertion)
 rdbm1 is a r1.size()*r1.size() array of difference bounds
 rdbm2 is a r2.size()*r2.size() array of difference bounds
 rdbm1 is a DBM over reference clocks r1
 rdbm2 is a DBM over reference clocks r2
 \return 0 if rdbm1 and rdbm2 are equal, a negative value if rdbm1 is smaller
 than rdbm2 w.r.t. lexical ordering, and a positive value otherwise
 */
int lexical_cmp(tchecker::dbm::db_t const * rdbm1, tchecker::reference_clock_variables_t const & r1,
                tchecker::dbm::db_t const * rdbm2, tchecker::reference_clock_variables_t const & r2);

} // end of namespace refdbm

} // end of namespace tchecker

#endif // TCHECKER_REFDBM_HH
