/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/zone_container.hh"

namespace tchecker {

std::shared_ptr<zone_container_t<tchecker::zg::zone_t>> contained_in_all(std::vector<zone_container_t<tchecker::zg::zone_t>> & zones, tchecker::clock_id_t dim)
{

  if (zones.empty()) {
    return std::make_shared<zone_container_t<tchecker::zg::zone_t>>(dim);
  }

  if (1 == zones.size()) {
    return std::make_shared<zone_container_t<tchecker::zg::zone_t>>(zones[0]);
  }

  zone_container_t<tchecker::zg::zone_t> cur = zones.back();
  zones.pop_back();

  std::shared_ptr<zone_container_t<tchecker::zg::zone_t>> inter = std::make_shared<zone_container_t<tchecker::zg::zone_t>>(*contained_in_all(zones, dim));

  std::shared_ptr<zone_container_t<tchecker::zg::zone_t>> result = std::make_shared<zone_container_t<tchecker::zg::zone_t>>(dim);

  for(auto iter_cur = cur.begin(); iter_cur < cur.end(); iter_cur++) {
    for(auto iter_inter = inter->begin(); iter_inter < inter->end(); iter_inter++) {
      std::shared_ptr<tchecker::zg::zone_t> tmp = tchecker::zg::factory(dim);
      if(tchecker::dbm::NON_EMPTY == tchecker::dbm::intersection(tmp->dbm(), (**iter_cur).dbm(), (**iter_inter).dbm(), (**iter_cur).dim())) {
        result->append_zone(tmp);
      }
    }
  }
  return result;
}

// specializations
template<>
std::shared_ptr<tchecker::zg::zone_t> zone_container_t<tchecker::zg::zone_t>::create_element()
{
  return tchecker::zg::factory(_dim);
}

template<>
std::shared_ptr<tchecker::zg::zone_t> zone_container_t<tchecker::zg::zone_t>::create_element(tchecker::zg::zone_t const & zone)
{
  return tchecker::zg::factory(zone);
}

template<>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element()
{
  return tchecker::virtual_constraint::factory(_dim);
}

template<>
std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t> zone_container_t<tchecker::virtual_constraint::virtual_constraint_t>::create_element(tchecker::virtual_constraint::virtual_constraint_t const & zone)
{
  return tchecker::virtual_constraint::factory(zone);
}


} // end of namespace tchecker
