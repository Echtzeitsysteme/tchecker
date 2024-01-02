/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/zone_container.hh"

namespace tchecker {

zone_container_t<tchecker::vcg::zone_t> contained_in_all(std::vector<zone_container_t<tchecker::vcg::zone_t>> & zones, tchecker::clock_id_t dim)
{

  if (zones.empty()) {
    zone_container_t<tchecker::vcg::zone_t> result(dim);
    return result;
  }

  if (1 == zones.size()) {
    return zones[0];
  }

  zone_container_t<tchecker::vcg::zone_t> cur = zones.back();
  zones.pop_back();

  zone_container_t<tchecker::vcg::zone_t> inter = contained_in_all(zones, dim);

  zone_container_t<tchecker::vcg::zone_t> result{dim};

  for(auto iter_cur = cur.begin(); iter_cur < cur.end(); iter_cur++) {
    for(auto iter_inter = inter.begin(); iter_inter < inter.end(); iter_inter++) {
      tchecker::vcg::zone_t *tmp = tchecker::vcg::factory(dim);
      if(tchecker::dbm::NON_EMPTY == tchecker::dbm::intersection(tmp->dbm(), (**iter_cur).dbm(), (**iter_inter).dbm(), (**iter_cur).dim())) {
        result.append_zone(tmp);
      }
      else {
        tchecker::zg::zone_t::destruct(tmp);
      }
    }
  }
  return result;
}

// specializations
template<>
tchecker::zg::zone_t * zone_container_t<tchecker::zg::zone_t>::create_element()
{
  return tchecker::zg::factory(_dim);
}

template<>
tchecker::zg::zone_t * zone_container_t<tchecker::zg::zone_t>::create_element(tchecker::zg::zone_t const & zone)
{
  return tchecker::zg::factory(zone);
}

template<>
tchecker::vcg::zone_t * zone_container_t<tchecker::vcg::zone_t>::create_element()
{
  return tchecker::vcg::factory(_dim);
}

template<>
tchecker::vcg::zone_t * zone_container_t<tchecker::vcg::zone_t>::create_element(tchecker::vcg::zone_t const & zone)
{
  return tchecker::vcg::factory(zone);
}

template<>
tchecker::virtual_constraint::virtual_constraint_t * zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element()
{
  return tchecker::virtual_constraint::factory(_dim);
}

template<>
tchecker::virtual_constraint::virtual_constraint_t * zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element(tchecker::virtual_constraint::virtual_constraint_t const & zone)
{
  return tchecker::virtual_constraint::factory(zone);
}


} // end of namespace tchecker
