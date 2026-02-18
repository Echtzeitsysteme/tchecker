/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_HH
#define TCHECKER_ZG_EXTRAPOLATION_HH

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/ta/system.hh"
/*!
 \file extrapolation.hh
 \brief Zone extrapolations to ensure finiteness of zone graphs
 */

namespace tchecker {

namespace zg {

/*!
 \class extrapolation_t
 \brief Zone extrapolation
 */
class extrapolation_t {
public:
  /*!
    \brief Destructor
   */
  virtual ~extrapolation_t() = default;

  /*!
   \brief Zone extrapolation
   \param dbm : a dbm
   \param dim : dimension of dbm
   \param vloc : a tuple of locations
   \post dbm has been extrapolated using clocks bounds in vloc
   */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) = 0;

  /*!
   \brief Accessor for the maximum value
   \return the maximum value used
   */
  virtual tchecker::clockbounds::bound_t max_value() const = 0;
};

/*!
 \class no_extrapolation_t
 \brief No zone extrapolation
*/
class no_extrapolation_t final : public tchecker::zg::extrapolation_t {
public:

  /*!
   \brief Constructor
   \note max_value is set to zero
  */
  no_extrapolation_t();  

  /*!
   \brief Constructor
   \param system : the system of the extrapolation
   \note The system is used to set the max_value
  */
  no_extrapolation_t(tchecker::ta::system_t const & system);

  /*!
   \brief Destructor
  */
  virtual ~no_extrapolation_t() = default;

  /*!
   \brief Zone extrapolation
   \param dbm : a dbm
   \param dim : dimension of dbm
   \param vloc : a tuple of locations
   \post dbm has not been modified
   */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);

  virtual tchecker::clockbounds::bound_t max_value() const override;

 private:
  tchecker::clockbounds::bound_t _max_value;
};

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_EXTRAPOLATION_HH
