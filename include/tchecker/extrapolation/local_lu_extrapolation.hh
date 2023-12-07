/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_LOCAL_LU_EXTRAPOLATION_HH
#define TCHECKER_ZG_EXTRAPOLATION_LOCAL_LU_EXTRAPOLATION_HH

#include "tchecker/extrapolation/extrapolation.hh"

namespace tchecker {

namespace zg {

namespace details {

/*!
 \class local_lu_extrapolation_t
 \brief Zone extrapolation with local LU clock bounds (abstract class)
*/
class local_lu_extrapolation_t : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : local LU clock bounds map
  */
  local_lu_extrapolation_t(std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> const & clock_bounds);

  /*!
  \brief Copy constructor
  */
  local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t const & e);

  /*!
  \brief Move constructor
  */
  local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t && e);

  /*!
   \brief Destructor
  */
  virtual ~local_lu_extrapolation_t();

  /*!
  \brief Assignment operator
  */
  tchecker::zg::details::local_lu_extrapolation_t & operator=(tchecker::zg::details::local_lu_extrapolation_t const & e);

  /*!
  \brief Move-assignment operator
  */
  tchecker::zg::details::local_lu_extrapolation_t & operator=(tchecker::zg::details::local_lu_extrapolation_t && e);

protected:
  tchecker::clockbounds::map_t * _l;                                          /*!< clock bounds L map */
  tchecker::clockbounds::map_t * _u;                                          /*!< clock bounds U map */
  std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> _clock_bounds; /*!< local LU clock bounds map */
};

} // end of namespace details

/*!
 \class local_extra_lu_t
 \brief ExtraLU zone extrapolation with local LU clock bounds
 */
class local_extra_lu_t final : public tchecker::zg::details::local_lu_extrapolation_t {
public:
  using tchecker::zg::details::local_lu_extrapolation_t::local_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~local_extra_lu_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU has been applied to dbm with local LU clock bounds in vloc
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

/*!
 \class local_extra_lu_plus_t
 \brief ExtraLU+ zone extrapolation with local LU clock bounds
 */
class local_extra_lu_plus_t final : public tchecker::zg::details::local_lu_extrapolation_t {
public:
  using tchecker::zg::details::local_lu_extrapolation_t::local_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~local_extra_lu_plus_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU+ has been applied to dbm with local LU clock bounds in vloc
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

} // end of namespace zg

} // end of namespace tchecker

#endif
