/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/vcg.hh"

namespace tchecker {

namespace vcg {

vcg_t::vcg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
              std::shared_ptr<tchecker::zg::semantics_t> const & semantics, tchecker::clock_id_t no_of_virtual_clocks,
              std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size)
  : tchecker::zg::zg_t(system, sharing_type, semantics, extrapolation, block_size, table_size),
    _no_of_virtual_clocks(no_of_virtual_clocks)
{
}

tchecker::clock_id_t vcg_t::get_no_of_virtual_clocks() const
{
  return _no_of_virtual_clocks;
}

tchecker::vcg::vcg_t * factory(std::shared_ptr<tchecker::strong_timed_bisim::system_virtual_clocks_t const> const & extended_system,
                               bool first_not_second,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_first,
                               std::shared_ptr<tchecker::ta::system_t const> const & orig_system_second,
                               enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                               enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                               std::size_t table_size) 
{

  std::shared_ptr<tchecker::zg::extrapolation_t> extrapolation{
    tchecker::vcg::extrapolation_factory(extrapolation_type, orig_system_first, orig_system_second, first_not_second)};

  if (extrapolation.get() == nullptr)
    return nullptr;

  std::shared_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};

  tchecker::clock_id_t vc = extended_system->get_no_of_virtual_clocks();

  return new tchecker::vcg::vcg_t(extended_system, sharing_type, semantics, vc, extrapolation, block_size, table_size);

}

zone_container_t contained_in_all(std::vector<zone_container_t> & zones, tchecker::clock_id_t dim)
{

  if (0 == zones.size()) {
    zone_container_t result(dim);
    return result;
  }

  if (1 == zones.size()) {
    return zones[0];
  }

  zone_container_t cur = zones.back();
  zones.pop_back();

  zone_container_t inter = contained_in_all(zones, dim);

  zone_container_t result{dim};

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

} // end of namespace vcg

} // end of namespace tchecker
