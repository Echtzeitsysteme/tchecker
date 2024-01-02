/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VCG_ZONE_HH
#define TCHECKER_VCG_ZONE_HH

#include "tchecker/zg/zone.hh"

namespace tchecker {

namespace vcg {

/*!
 \file zone.hh
 \brief DBM implementation of zones for vcgs
 */
class zone_t : public tchecker::zg::zone_t {

public:

private:
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
  zone_t(tchecker::zg::zone_t const & zone) = delete;

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
 \param zone : the zone to copy
 */
zone_t * factory(zone_t const & zone);

} // end of namespace vcg

} // end of namespace tchecker

#endif
