/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_K_NORM_HH
#define TCHECKER_ZG_EXTRAPOLATION_K_NORM_HH

#include "tchecker/extrapolation/extrapolation.hh"
#include "tchecker/extrapolation/global_lu_extrapolation.hh"

namespace tchecker {

namespace zg {

/*!
 \class k_norm
 \brief K-Normalization for zone extrapolation
 */
class k_norm final : public tchecker::zg::global_extra_lu_t {
public:

  /*!
   \brief Constructor
   \param clock_bounds : global LU clock bounds map
   \note WARNING: we change the clock bounds, ignoring the const statement
  */
  k_norm(std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds);

  /*!
  \brief Destructor
  */
  virtual ~k_norm() = default;

  using tchecker::zg::global_extra_lu_t::extrapolate;

};

} // end of namespace zg

namespace vcg {

class k_norm_virtual final : public tchecker::zg::global_extra_lu_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : global LU clock bounds map
   \note WARNING: we change the clock bounds, ignoring the const statement
  */
  k_norm_virtual(std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds);

  /*!
  \brief Destructor
  */
  virtual ~k_norm_virtual() = default;

  using tchecker::zg::global_extra_lu_t::extrapolate;

};

} // end of namespace vcg

} // end of namespace tchecker

#endif
