/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/zone.hh"

namespace tchecker {

namespace vcg {

vcg::zone_t * factory(tchecker::clock_id_t dim)
{
  return reinterpret_cast<tchecker::vcg::zone_t *>(tchecker::zg::factory(dim));
}

vcg::zone_t * factory(tchecker::vcg::zone_t const & zone)
{
  tchecker::zg::zone_t const & reinterpreted_zone = reinterpret_cast<tchecker::zg::zone_t const &>(zone);
  return reinterpret_cast<tchecker::vcg::zone_t *>(tchecker::zg::factory(reinterpreted_zone));
}


} // end of namespace vcg

} // end of namespace tchecker
