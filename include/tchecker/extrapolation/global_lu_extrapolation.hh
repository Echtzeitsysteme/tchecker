/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_GLOBAL_LU_EXTRAPOLATION_HH
#define TCHECKER_ZG_EXTRAPOLATION_GLOBAL_LU_EXTRAPOLATION_HH

#include "tchecker/extrapolation/extrapolation.hh"

namespace tchecker {

namespace zg {

namespace details {

/*!
 \class global_lu_extrapolation_t
 \brief Zone extrapolation with global LU clock bounds (abstract class)
*/
class global_lu_extrapolation_t : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : global LU clock bounds map
  */
  global_lu_extrapolation_t(std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds);

  /*!
  \brief Copy constructor
  */
  global_lu_extrapolation_t(tchecker::zg::details::global_lu_extrapolation_t const & e) = default;

  /*!
  \brief Move constructor
  */
  global_lu_extrapolation_t(tchecker::zg::details::global_lu_extrapolation_t && e) = default;

  /*!
   \brief Destructor
  */
  virtual ~global_lu_extrapolation_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::zg::details::global_lu_extrapolation_t &
  operator=(tchecker::zg::details::global_lu_extrapolation_t const & e) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::zg::details::global_lu_extrapolation_t & operator=(tchecker::zg::details::global_lu_extrapolation_t && e) = default;

  virtual tchecker::clockbounds::bound_t max_value() const override;

protected:
  std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> _clock_bounds; /*!< global LU clock bounds map */
};

} // end of namespace details

/*!
 \class global_extra_lu_t
 \brief ExtraLU zone extrapolation with global LU clock bounds
 */
class global_extra_lu_t : public tchecker::zg::details::global_lu_extrapolation_t {
public:
  using tchecker::zg::details::global_lu_extrapolation_t::global_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~global_extra_lu_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU has been applied to dbm with global LU clock bounds
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) final;
};

/*!
 \class global_extra_lu_plus_t
 \brief ExtraLU+ zone extrapolation with global LU clock bounds
 */
class global_extra_lu_plus_t final : public tchecker::zg::details::global_lu_extrapolation_t {
public:
  using tchecker::zg::details::global_lu_extrapolation_t::global_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~global_extra_lu_plus_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU+ has been applied to dbm with global LU clock bounds
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

} // enf of namespace zg

} // end of namespace tchecker

#endif
