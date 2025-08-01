/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_DBM_DBM_HH
#define TCHECKER_DBM_DBM_HH

#include <functional>
#include <iostream>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file dbm.hh
 \brief DBMs and operations on DBMs
 \note A DBM is a symbolic representation for conjunctions of difference constraints like x-y#c with
 # either < or <=. This implementation uses DBMs to represent sets of clock valuations. We assume a
 special clock 0 that represents the value 0. A DBM of dimension dim hence has clock 0 and dim-1 actual
 clocks with indices 1 to dim-1.
 A DBM of dimension dim is represented as a dim*dim array of difference bounds tchecker::dbm::db_t.
 The difference bound #c in [i,j] encodes the constraint x_i - x_j # c. The difference bound in [i,j] for
 dbm of dimension dim is accessed with dbm[i*dim+j].
 A DBM can be seen as weighted graph where nodes are clocks (including 0) and for any two clocks x and y,
 the weigth of the edge x -> y is the difference bound #c of the constraint y - x #c in the DBM.
 A DBM is consistent if for every clock x, the bound for x - x is <=0, and if the bound for 0 - x is less-than
 or equal-to <=0 (i.e. clocks are positive or zero). Notice that consistent zones are not empty.
 A DBM is tight if for every constraint x - y # c, the bound #c is the shortest-path from y to x in the
 corresponding graph. Every non-empty DBM has a unique corresponding tight DBM. Empty DBMs have a negative
 cycle on some clock x. Hence they cannot be tightened: there is no shortest path from x to x.
 */

namespace tchecker {

namespace dbm {

/*!
 \brief Status of a DBM
 */
enum status_t {
  EMPTY = 0,
  NON_EMPTY,
  MAY_BE_EMPTY,
};

/*!
 \brief get dbm[i, j]
 \return a pointer to dbm[i, j]
 */
const tchecker::dbm::db_t * access(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t i, tchecker::clock_id_t j);

/*!
 \brief non const version of access
 \return a pointer to dbm[i, j]
 \note Watch Out what you are doing! There is no guarantee on anything!
 */
tchecker::dbm::db_t * access(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t i, tchecker::clock_id_t j);

/*!
 \brief Copy a DBM into another DBM
 \param dbm1 : target dbm
 \param dbm2 : source dbm
 \param dim : dimension of dbm1 and dbm2
 \pre dbm1 is not nullptr (checked by assertion)
 dbm1 is a dim*dim array of difference bounds
 dbm2 is not nullptr (checked by assertion)
 dbm2 is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm1 is a copy of dbm2
*/
void copy(tchecker::dbm::db_t * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);

/*!
 \brief Universal zone
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the true/universal zone (all valuations, positive and negative)
 dbm is tight.
 */
void universal(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Universal positive zone
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the positive true/universal zone (i.e. conjunction x >= 0 for all clocks x)
 dbm is tight.
 */
void universal_positive(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Empty zone
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the empty zone (is_empty_0() returns true on dbm)
 dbm IS NOT TIGHT (empty DBMs cannot be tight)
 */
void empty(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Initialize to zero
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \post dbm represents the zone that contains the valuation 0 in dimension dim.
 dbm is tight.
 */
void zero(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Consistency checking
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of tchecker::dbm::db_t.
 dim >= 1 (checked by assertion)
 \return true if the diagonal in dbm is <=0, and false otherwise
 */
bool is_consistent(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Fast emptiness predicate
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm has been tightened
 dim >= 1 (checked by assertion).
 \return true if dbm has a negative difference bound on (0,0), false otherwise
 \note this function only checks the (0,0) entry of dbm. Hence, dbm may be empty but not be detected by this function. However,
 all other functions set (0,0) to a value less-than <=0 when they generate an empty zone. So this function can be used as a
 safe and efficient emptiness check if dbm has been generated by calls to functions in this file.
 */
bool is_empty_0(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Universality predicate
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion).
 \return true if dbm represents the true/universal zone, false otherwise (see tchecker::dbm::universal)
 */
bool is_universal(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Positivity predicate
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion).
 \return true if dbm is positive (i.e. for every valuation in dbm, and every clock x, x>=0), false otherwise
 */
bool is_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Positivity & universality predicate
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion).
 \return true if dbm is the set of all positive valuations, false otherwise
 \note efficient algorithm for tchecker::dbm::is_universal(dbm, dim) && tchecker::dbm::is_positive(dbm dim)
 */
bool is_universal_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Check if a DBM contains the zero valuation
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 \return true if dbm contains the valuation where all clocks have value zero,
 false otherwise
*/
bool contains_zero(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Tightness predicate
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \return true if dbm is tight, false otherwise
 */
bool is_tight(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Tighten a DBM
 \param dbm : a DBM
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 \post dbm is not nullptr (checked by assertion)
 dbm is tight if it is not empty.
 if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
 \return EMPTY if dbm is empty, NON_EMPTY otherwise
 \note Applies Floyd-Warshall algorithm on dbm seen as a weighted graph.
 */
enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Tighten a DBM w.r.t. a constraint
 \param dbm : a DBM
 \param dim : dimension of dbm
 \param x : first clock
 \param y : second clock
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 0 <= x < dim (checked by assertion)
 0 <= y < dim (checked by assertion)
 x != y (checked by assertion)
 \post for all clocks u and v, the edge u->v in the graph is tight w.r.t. the
 edge y->x. That is, the length of the path u->v is the minimum between its
 length before the call and the length of the path u->y->x->v.
 if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
 \return EMPTY if dbm is empty, MAY_BE_EMPTY otherwise
 \note if every edge in dbm is tight w.r.t. all other edges except i->j, then after the call, dbm is either empty, or it is
 not empty and tight.
 */
enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                                     tchecker::clock_id_t y);

/*!
 \brief Constrain a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : first clock
 \param y : second clock
 \param cmp : constraint comparator
 \param value : constraint value
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 0 <= y < dim (checked by assertion)
 value can be represented in a tchecker::dbm::db_t
 \post dbm has been intersected with constraint `x - y # value` where # is < if cmp is LT, and # is <= if cmp is LE
 dbm is tight if it is not empty.
 if dbm is empty, then its difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
 \return EMPTY if dbm is empty, NON_EMPTY otherwise
 \throw std::invalid_argument : if `cmp value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
 DBM_UNSAFE is not set)
 */
enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                                       tchecker::clock_id_t y, tchecker::ineq_cmp_t cmp, tchecker::integer_t value);

/*!
 \brief Constrain a DBM w.r.t. a clock constraint
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param cc : clock constraint
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 cc is expressed over system clocks (i.e. the reference clock is tchecker::REFCLOCK_ID)
 \post dbm has been intersected with cc
 if dbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::dbm::is_empty_0() returns true)
 the resulting DBM is tight and consistent if not empty
 \return EMPTY is the resulting DBM is empty, NON_EMPTY otherwise
*/
enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_t const & cc);

/*!
 \brief Constrain a DBM w.r.t. clock constraints container
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param constraints : clock constraints
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 all clock constraints in constraints are expressed over system clocks
 \post dbm has been intersected with constraints
 if dbm is empty, then its difference bound in (0,0) is less-than <=0
 (tchecker::dbm::is_empty_0() returns true)
 the resulting DBM is tight and consistent if not empty
 \return EMPTY is the resulting DBM is empty, NON_EMPTY otherwise
*/
enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_container_t const & constraints);

/*!
 \brief Equality predicate
 \param dbm1 : a first dbm
 \param dmb2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \return true if dbm1 and dbm2 are equal, false otherwise
 */
bool is_equal(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);

/*!
 \brief Inclusion predicate
 \param dbm1 : a first dbm
 \param dmb2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \return true if dbm1 is included into dbm2, false otherwise
 */
bool is_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);

/*!
 \brief Check if a DBM satisfies a clock constraint
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : first clock
 \param y : second clock
 \param cmp : constraint comparator
 \param value : constraint value
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 x < dim (checked by assertion)
 y < dim (checked by assertion)
 value can be represented in a tchecker::dbm::db_t
 \return true if all valuations in dbm satisfy "x - y cmp value", false otherwise
 */
bool satisfies(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
               tchecker::ineq_cmp_t cmp, tchecker::integer_t value);

/*!
 \brief Check if a DBM satisfies a clock constraint
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param c : clock constraint
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 c is expressed over system clocks (i.e. the reference clock is tchecker::REFCLOCK_ID)
 \return true if all valuations in dbm satisfy c, false otherwise
 */
bool satisfies(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clock_constraint_t const & c);

/*!
 \brief Reset a clock
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : left-value clock
 \param y : right-value clock
 \param value : a value
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 0 <= y < dim (checked by assertion)
 0 <= value (checked by assertion)
 `<= value` can be represented by tchecker::dbm::db_t (checked by assertion)
 \post dbm has been updated according to reset `x := y + value`
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 \note wrapper for dedicated functions depending on y and value. Set y to 0 in order to reset x to constant value. Set value
 to 0 in order to reset x to y. Set both y and value non-zero to reset x to the sum y+value. Increment x by value if y==x.
 \throw std::invalid_argument : if `<= value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
 DBM_UNSAFE is not set)
 */
void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
           tchecker::integer_t value);

/*!
 \brief Reset from a clock reset container
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param resets : a clock reset container
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 all resets are expressed over system clocks
 \post dbm has been updated according to the sequence of resets
 dbm is tight and consistent
 */
void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_container_t const & resets);

/*!
 \brief Reset a clock to a constant
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : left-value clock
 \param value : a value
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 0 <= value *checked by assertion)
 `<= value` can be represented by tchecker::dbm::db_t
 \post dbm has been updated according to reset x := value.
 dbm is consistent.
 dbm is tight.
 \throw std::invalid_argument : if `<= value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
 DBM_UNSAFE is not set)
 */
void reset_to_value(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::integer_t value);

/*!
 \brief Reset a clock to another clock
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : left-value clock
 \param y : right-value clock
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 0 <  y < dim (checked by assertion)
 \post dbm has been updated according to reset x := y.
 dbm is consistent.
 dbm is tight.
 */
void reset_to_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y);

/*!
 \brief Reset a clock to the sum of a clock and a non-negative integer
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : left-value clock
 \param y : right-value clock
 \param value : a value
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 0 <= y < dim (checked by assertion)
 0 <= value (checked by assertion)
 `<= value` can be represented by tchecker::dbm::db_t (checked by assertion)
 \post dbm has been updated according to reset `x := y + value`
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 */
void reset_to_sum(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
                  tchecker::integer_t value);

/*!
 \brief Free clock value (reverse reset)
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : clock
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 0 <= x < dim (checked by assertion)
 \post dbm represents the set of clock valuations v such that v[x:=d] belongs to
 the original dbm for some real value d
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
*/
void free_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x);

/*!
 \brief Free clock value (reverse reset)
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param reset : a clock reset
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 reset.left_id() is not tchecker::REFCLOCK_ID (checked by assertion)
 \post dbm represents the set of clock valuations v such that v[reset] belongs to
 the original dbm for some real value d
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
*/
void free_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_t const & reset);

/*!
 \brief Free clock value (reverse reset)
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param resets : a sequence of clock resets
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 for every reset in resets, reset.left_id() is not tchecker::REFCLOCK_ID (checked by assertion)
 \post dbm represents the set of clock valuations v such that v[resets] belongs to
 the original dbm for some real value d
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
*/
void free_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_container_t const & resets);

/*!
 \brief Open up (delay)
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \post all upper bounds on clocks in dbm are <infinity.
 dbm is tight.
 */
void open_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Open down (reverse delay)
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \post dbm represents the set of clock valuations v s.t.
 v+d belongs to the original dbm for some delay d
 dbm is tight.
*/
void open_down(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Intersection
 \param dbm : a dbm
 \param dbm1 : a dbm
 \param dbm2 : a dbm
 \param dim : dimension of dbm1, dbm1 and dbm2
 \pre dbm, dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm, dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are consistent (checked by assertion)
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \post dbm is the intersection of dbm1 and dbm2
 dbm is consistent
 dbm is tight
 \return EMPTY if the intersection of dbm1 and dbm2 is empty, NON_EMPTY otherwise
 \note dbm can be one of dbm1 or dbm2
 */
enum tchecker::dbm::status_t intersection(tchecker::dbm::db_t * dbm, tchecker::dbm::db_t const * dbm1,
                                          tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);


/*!
 \brief disjoint
 \param dbm1 : a dbm
 \param dbm2 : a dbm
 \param dim : dimension of dbm1 and dbm2
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are consistent (checked by assertion)
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion).
 \return [dbm1 && dbm2] == emptyset
 */
bool disjoint(tchecker::dbm::db_t * dbm_1, tchecker::dbm::db_t * dbm2, tchecker::clock_id_t dim);

/*!
 \brief revert-multiple-reset function (see the TR of Lieb et al.)
 \param result : where the result will be stored (must be allocated)
 \param orig_zone : the previous dbm
 \param dim : the dimension of the used dbm
 \param zone_split : the split of reset(orig_zone)
 \param reset : the used reset set
 \return void
 */
enum tchecker::dbm::status_t revert_multiple_reset(tchecker::dbm::db_t *result, const tchecker::dbm::db_t * orig_zone,
                                                   tchecker::clock_id_t dim, tchecker::dbm::db_t * zone_split,
                                                   tchecker::clock_reset_container_t reset);

/*!
 \brief ExtraM extrapolation
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param m : clock bounds for clocks 1 to dim-1 (m[0] is the bound for clock 1 and so on)
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is positive (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 m is an array of size dim-1
 m[i] < tchecker::dbm::INF_VALUE for all i>=0 (checked by assertion)
 \post extrapolation ExtraM has been applied to dbm w.r.t. clock
 bounds m (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and Pelanek.
 Int. J. STTT, 2006)
 \note set m[i] to -tchecker::dbm::INF_VALUE if clock i has no bound
 */
void extra_m(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * m);

/*!
 \brief ExtraM+ extrapolation
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param m : clock bounds for clocks 1 to dim-1 (m[0] is the bound for clock 1 and so on)
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is positive (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 m is an array of size dim-1
 m[i] < tchecker::dbm::INF_VALUE for all i>=0 (checked by assertion)
 \post extrapolation ExtraM+ has been applied to dbm w.r.t. clock
 bounds m (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and Pelanek.
 Int. J. STTT, 2006)
 \note set m[i] to -tchecker::dbm::INF_VALUE if clock i has no bound
 */
void extra_m_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * m);

/*!
 \brief ExtraLU extrapolation
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param l : clock lower bounds for clocks 1 to dim-1 (l[0] is the bound for clock 1 and so on)
 \param u : clock upper bounds for clocks 1 to dim-1 (u[0] is the bound for clock 1 and so on)
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is positive (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 l and u are arrays of size dim-1
 l[i], u[i] < tchecker::dbm::INF_VALUE for all i>=0 (checked by assertion)
 \post extrapolation ExtraLU has been applied to dbm w.r.t. clock
 bounds l and u (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and
 Pelanek. Int. J. STTT, 2006)
 \note set l[i]/u[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
 */
void extra_lu(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * l,
              tchecker::integer_t const * u);

/*!
 \brief ExtraLU+ extrapolation
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param l : clock lower bounds for clocks 1 to dim-1 (l[0] is the bound for clock 1 and so on)
 \param u : clock upper bounds for clocks 1 to dim-1 (u[0] is the bound for clock 1 and so on)
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is positive (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 l and u are arrays of size dim-1
 l[i], u[i] < tchecker::dbm::INF_VALUE for all i>=0 (checked by assertion)
 \post extrapolation ExtraLU+ has been applied to dbm w.r.t. clock
 bounds l and u (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and
 Pelanek. Int. J. STTT, 2006)
 \note set l[i]/u[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
 */
void extra_lu_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * l,
                   tchecker::integer_t const * u);

/*!
 \brief Checks inclusion w.r.t. abstraction aLU
 \param dbm1 : a first dbm
 \param dbm2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \param l : clock lower bounds for clocks 1 to dim-1 (l[0] is the bound for clock 1 and so on)
 \param u : clock upper bounds for clocks 1 to dim-1 (u[0] is the bound for clock 1 and so on)
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are consistent (checked by assertion)
 dbm1 and dbm2 are positive (checked by assertion)
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion)
 l and u are arrays of size dim-1
 l[i], u[i] < tchecker::dbm::INF_VALUE for all i>=0 (checked by assertion)
 \return true if dbm1 <= aLU(dbm2), false otherwise (see "Better abstractions for timed automata", Herbreteau, Srivathsan
 and Walukiewicz. Inf. Comput., 2016)
 \note set l[i]/u[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
 */
bool is_alu_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
               tchecker::integer_t const * l, tchecker::integer_t const * u);

/*!
 \brief Checks inclusion w.r.t. abstraction aM
 \param dbm1 : a first dbm
 \param dbm2 : a second dbm
 \param dim : dimension of dbm1 and dbm2
 \param m : clock bounds for clocks 1 to dim-1 (m[0] is the bound for clock 1 and so on)
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 and dbm2 are dim*dim arrays of difference bounds
 dbm1 and dbm2 are consistent (checked by assertion)
 dbm1 and dbm2 are positive (checked by assertion)
 dbm1 and dbm2 are tight (checked by assertion)
 dim >= 1 (checked by assertion)
 m is an array of size dim-1
 m[i] < tchecker::dbm::INF_VALUE for all i>=0 (checked by assertion)
 \return true if dbm1 <= aM(dbm2), false otherwise (see "Better abstractions for timed automata", Herbreteau, Srivathsan
 and Walukiewicz. Inf. Comput., 2016)
 \note set m[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
 */
bool is_am_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
              tchecker::integer_t const * m);

/*!
 \brief Hash function
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 \return hash code of dbm
 */
std::size_t hash(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Output a DBM as a matrix
 \param os : output stream
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion).
 \post dbm has been output to os
 \return os after output
 */
std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Output a DBM as a conjunction of constraints
 \param os : output stream
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param clock_name : map from clock IDs to strings
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion).
 clock_name maps any clock ID in [0,dim) to a name
 \post the relevant constraints in dbm has been output to os. Relevant constraints are those that differ from the
 universal DBM.
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                      std::function<std::string(tchecker::clock_id_t)> clock_name);

/*!
 \brief Lexical ordering
 \param dbm1 : first dbm
 \param dim1 : dimension of dbm1
 \param dbm2 : second dbm
 \param dim2 : dimension of dbm2
 \pre dbm1 and dbm2 are not nullptr (checked by assertion)
 dbm1 is a dim1*dim1 array of difference bounds
 dbm2 is a dim2*dim2 array of difference bounds
 dim1 >= 1 and dim2 >= 1 (checked by assertion)
 \return 0 if dbm1 and dbm2 are equal, a negative value if dbm1 is smaller than dbm2 w.r.t. lexical ordering, and a positive
 value otherwise
 */
int lexical_cmp(tchecker::dbm::db_t const * dbm1, tchecker::clock_id_t dim1, tchecker::dbm::db_t const * dbm2,
                tchecker::clock_id_t dim2);

/*!
 \brief Multiply all constants of a dbm by factor
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param factor scale factor
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 factor > 0
 \post all non infinity values in dbm have been multiplied by factor
 dbm is consistent and tight
 \throw std::invalid_argument : if factor <= 0
 \throw std::overflow_error : if overflow occurs
 \throw std::underflow_error : if underflow occurs
 \note (<,inf) in dbm are left unchanged
 */
void scale_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t factor);

/*!
 \brief Divide all constants of a dbm by factor
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param factor scale factor
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 factor > 0
 factor is a divider of all non-infinity entries in dbm
 \post all non infinity values in dbm have been divided by factor
 dbm is consistent and tight
 \throw std::invalid_argument : if factor <= 0 or if factor is not a divider of all entries in dbm
 \note (<,inf) in dbm are left unchanged
 */
void scale_down(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t factor);

/*!
 \brief Check if a clock has a fixed value in a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : clock ID
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 0<=x<dim (checked by assertion)
 \return true if clock x has a fixed value in dbm, false otherwise
*/
bool has_fixed_value(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x);

/*!
 \brief Check if a clock admits an integer value in a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x : clock ID
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 0<=x<dim (checked by assertion)
 \return true if clock x admits an integer value in dbm, false otherwise
*/
bool admits_integer_value(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x);

/*!
 \brief Check if a DBM contains a single valuation
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dim >= 1 (checked by assertion)
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 \return true if dbm contains a single valuation, false otherwise
*/
bool is_single_valuation(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Constrains a DBM to a single valuation
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dbm is positive (checked by assertion)
 dim >= 1 (checked by assertion)
 \post dbm represents a single valuation
 dbm is consistent and tight
 \return the scale factor applied to dbm to make it single integer valued
 \throw std::overflow_error : if overflow occurs
 \throw std::underflow_error : if underflow occurs
 \note A DBM can only represent integer values, whereas constraints such as 0<x<1
 only admit non-integer valuations. The solution is thus to scale up dbm by some
 factor to enlarge constraints in such a way that they admit an integer solution
 (e.g. scale up by 2 yields 0<x<2 which admits 1 as an integer solution). The returned
 value is the scale factor applied to dbm in order to get an integer solution.
 Hence, the actual rational valuation consists in clock values in the DBM divided by
 the scale factor.
*/
tchecker::integer_t constrain_to_single_valuation(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);

/*!
 \brief Compute a satisfying integer valuation from a single valuation DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param clock_values : vector of clock_values
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dbm is positive (checked by assertion)
 dbm is single valuation (checked by assertion)
 dim >= 1 (checked by assertion)
 clock_values has size dim (checked by assertion)
 \post clock_values contains the integer clock valuation that satisfies dbm
*/
void satisfying_integer_valuation(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                  std::vector<tchecker::integer_t> & clock_values);

/*!
 \brief Compute greatest common divisor of entries in a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 \return the greatest common divisor of all non-infinity entries in rdbm, 0 if
 all non-infinity entries are 0
*/
tchecker::integer_t gcd(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);

/*!
 \brief Check is a clock valuation satisfies a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 the size of clockval is equal to dim (checked by assertion)
 \return true if clockval satisfies all the constraints in dbm, false otherwise
 */
bool satisfies(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clockval_t const & clockval);

/*!
\brief Type of clock ordering
*/
enum clock_ordering_t {
  CLK_LT,           /*!< Less-than */
  CLK_LE,           /*!< Less-than or equal-to */
  CLK_EQ,           /*!< Equal */
  CLK_GE,           /*!< Greater-than or equal-to */
  CLK_GT,           /*!< Greater-than */
  CLK_INCOMPARABLE, /*!< Incomparable */
};

/*!
 \brief Check ordering of two clocks in a DBM
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x1 : a first clock
 \param x2 : a second clock
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 x1 and x2 are less than dim (checked by assertion)
 \return tchecker::dbm::CLK_LT if x1 < x2 is true in all valuations in dbm,
 tchecker::dbm::CLK_LE if x1 <= x2 is true in all valuations in dbm
 tchecker::dbm::CLK_EQ if x1 = x2 is true in all valuations in dbm
 tchecker::dbm::CLK_GE if x1 >= x2 is true in all valuations in dbm
 tchecker::dbm::CLK_GT if x1 > x2 is true in all valuations in dbm
 tchecker::dbm::CLK_INCOMPARABLE otherwise
*/
enum tchecker::dbm::clock_ordering_t clock_cmp(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                                               tchecker::clock_id_t x1, tchecker::clock_id_t x2);

/*!
 \brief Type of clock relative position in time
*/
enum clock_position_t {
  CLK_AHEAD,         /*!< Clock is strictly ahead in time */
  CLK_BEHIND,        /*!< Clock is strictly behind in time */
  CLK_SYNCHRONIZED,  /*!< Clocks are synchronized */
  CLK_SYNCHRONIZABLE /*!< Clocks are synchronizable */
};

/*!
 \brief Check clocks relative position in time
 \param dbm : a dbm
 \param dim : dimension of dbm
 \param x1 : a first clock
 \param x2 : a second clock
 \pre dbm is not nullptr (checked by assertion)
 dbm is a dim*dim array of difference bounds
 dbm is consistent (checked by assertion)
 dbm is tight (checked by assertion)
 dim >= 1 (checked by assertion)
 x1 and x2 are less than dim (checked by assertion)
 \return tchecker::dbm::CLK_AHEAD if clock x1 is greater than clock x2 in all the valuations in dbm
 tchecker::dbm::CLK_BEHIND if clock x1 is less than clocks x2 in all the valuations in dbm
 tchecker::dbm::CLK_SYNCHRONIZED if clock x1 and x2 are equal in all the valuations in dbm
 tchecker::dbm::CLK_SYNCHRONIZABLE otherwise, i.e.there exists a valuation in dbm where x1 and x2 are equal
 \note this function is quite similar to tchecker::dbm::clock_cmp, however it allows to think about relative
 positions of clocks on a timeline, independently of representation of time in DBMs
 */
enum tchecker::dbm::clock_position_t clock_position(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                                                    tchecker::clock_id_t x1, tchecker::clock_id_t x2);

/*
 \brief Return value of convex-union
*/
enum union_convex_t {
  UNION_IS_CONVEX,      /*!< the union of two dbm can be represented as a dbm */
  UNION_IS_NOT_CONVEX   /*!< the union of two dbm cannot be represented as a dbm */
};

/*!
 \brief tries to union two dbms
 \param result : where the result will be stored (must be allocated!)
 \param dbm1 : a dbm
 \param dbm2 : a dbm to be unioned with dbm1
 \param dim : the dimension (must be the same for all dbm!)
 \pre none of the dbm is nullptr (checked by assertion)
 dbm1 and dbm1 are consistent and tight (checked by assertion)
 dim >= 1 (checked by assertion)
 \return UNION_IS_CONVEX if the union of dbm1 and dbm2 can be represented by a dbm. UNION_IS_NOT_CONVEX otherwise
 \post if UNION_IS_CONVEX is returned, result contains the union.
 \note This function implements the convex-union algorithm from the dissertation "Rokicki, Tomas Gerhard: Representing and modeling digital circuits, Stanford University, 1994"
 */
enum tchecker::dbm::union_convex_t convex_union(tchecker::dbm::db_t *result, tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);

} // end of namespace dbm

} // end of namespace tchecker

#endif // TCHECKER_DBM_DBM_HH
