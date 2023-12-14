/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace vcg {

virtual_constraint_t::virtual_constraint_t(const clock_constraint_container_t & vcs, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks)
 : _dim(no_of_virtual_clocks + 1), _lowest_virtual_clk_index(dim - no_of_virtual_clocks), _vcs(vcs)
{
}

const clock_constraint_container_t & virtual_constraint_t::get_vc() const {
  return _vcs;
}

virtual_constraint_t * factory(const tchecker::zg::zone_t * zone, tchecker::clock_id_t no_of_virtual_clocks)
{
  return factory(zone->dbm(), zone->dim(), no_of_virtual_clocks);
}

virtual_constraint_t * factory(const tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t no_of_virtual_clocks)
{
  std::vector<tchecker::clock_id_t> indices;
  indices.emplace_back(0);

  for(tchecker::clock_id_t  i = dim - no_of_virtual_clocks; i < dim; ++i)
  {
    indices.emplace_back(i);
  }

  std::vector<clock_constraint_t> vcs;

  for(std::size_t i = 0; i < indices.size(); ++i)
  {
    for(std::size_t j = 0; j < indices.size(); ++j)
    {
      clock_constraint_t cur_vc{indices[i], indices[j], dbm[indices[i]*dim + indices[j]].cmp, dbm[indices[i]*dim + indices[j]].value};
      vcs.push_back(cur_vc);
    }
  }

  return new virtual_constraint_t(vcs, dim, no_of_virtual_clocks);
}

} // end of namespace vcg

} // end of namespace tchecker
