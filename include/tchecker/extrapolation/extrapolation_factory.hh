/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_FACTORY_HH
#define TCHECKER_ZG_EXTRAPOLATION_FACTORY_HH

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/extrapolation/extrapolation.hh"

/*!
 \file extrapolation.hh
 \brief Zone extrapolations to ensure finiteness of zone graphs
 */

namespace tchecker {

namespace zg {

/*!
 \brief Type of extrapolation
*/
enum extrapolation_type_t {
  NO_EXTRAPOLATION,     /*!< see tchecker::zg::no_extrapolation_t */
  EXTRA_LU_GLOBAL,      /*!< see tchecker::zg::global_extra_lu_t */
  EXTRA_LU_LOCAL,       /*!< see tchecker::zg::local_extra_lu_t */
  EXTRA_LU_PLUS_GLOBAL, /*!< see tchecker::zg::global_extra_lu_plus_t */
  EXTRA_LU_PLUS_LOCAL,  /*!< see tchecker::zg::local_extra_lu_plus_t */
  EXTRA_M_GLOBAL,       /*!< see tchecker::zg::global_extra_m_t */
  EXTRA_M_LOCAL,        /*!< see tchecker::zg::local_extra_m_t */
  EXTRA_M_PLUS_GLOBAL,  /*!< see tchecker::zg::global_extra_m_plus_t */
  EXTRA_M_PLUS_LOCAL,   /*!< see tchecker::zg::local_extra_m_plus_t */
  EXTRA_K_NORM          /* see tchecker::zg::k_norm */
};

/*!
 \brief Zone extrapolation factory
 \param extrapolation_type : type of extrapolation
 \param system : system of timed processes
 \return a zone extrapolation of type extrapolation_type using clock bounds
 inferred from system, nullptr if clock bounds cannot be inferred from system (see
 tchecker::clockbounds::compute_clockbounds)
 \note the returned extrapolation must be deallocated by the caller
 \throw std::invalid_argument : if extrapolation_type is unknown
 */
tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::ta::system_t const & system);

/*!
 \brief Zone extrapolation factory
 \param extrapolation_type : type of extrapolation
 \param clock_bounds : clock bounds
 \return a zone extrapolation of type extrapolation_type using clock bounds from
 clock_bounds
 \note the returned extrapolation must be deallocated by the caller
 \throw std::invalid_argument : if extrapolation_type is unknown
 */
tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::clockbounds::clockbounds_t const & clock_bounds);

} // end of namespace zg

namespace vcg {

/*!
 \brief Zone extrapolation factory
 \param extrapolation_type : type of extrapolation
 \param orig_system_first : first system of timed processes
 \param orig_system_secpmd : second system of timed processes
 \param first_not_second : true iff this vcg is the left hand side of the comparison
 \return a zone extrapolation of type extrapolation_type using clock bounds
 inferred from the systems, nullptr if clock bounds cannot be inferred from system (see
 tchecker::clockbounds::compute_clockbounds)
 \note the returned extrapolation must be deallocated by the caller
 \throw std::invalid_argument : if extrapolation_type is unknown
 */
tchecker::zg::extrapolation_t * extrapolation_factory(
                  enum tchecker::zg::extrapolation_type_t type,
                  std::shared_ptr<tchecker::ta::system_t const> const & orig_system_first,
                  std::shared_ptr<tchecker::ta::system_t const> const & orig_system_second,
                  bool first_not_second);

} // end of namespace vcg

} // end of namespace tchecker

#endif // TCHECKER_ZG_EXTRAPOLATION_HH
