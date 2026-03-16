/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/init_states_creator.hh"

#include "tchecker/basictypes.hh"
#include "tchecker/vcg/virtual_constraint.hh"

namespace tchecker {

namespace strong_timed_bisim {

void init(const std::shared_ptr<tchecker::vcg::vcg_t> vcg, 
          std::map<std::string, std::string> & starting_state, std::vector<tchecker::zg::zg_t::sst_t> & sst)
{
  sst.clear();
  if(starting_state.empty()) {
    vcg->initial(sst);
  } else {
    vcg->build(starting_state, sst);
  }

  if(sst.size() != 1 || tchecker::STATE_OK != std::get<0>(sst[0])) {
    throw std::runtime_error("problems with initial state. Exactly a single initial state for each TA needed.");
  }

}

std::shared_ptr<tchecker::virtual_constraint::virtual_constraint_t>
create_and_apply_vc(const std::shared_ptr<tchecker::vcg::vcg_t> vcg, tchecker::zg::zone_t & zone, tchecker::clock_id_t add_to_id) 
{
  for(tchecker::clock_id_t i = 0; i < vcg->get_no_of_original_clocks(); ++i) {
    std::string virt_name = std::string("chi") + std::to_string(i + add_to_id);
    tchecker::clock_id_t virt_id = vcg->system().clock_id(virt_name);

    tchecker::dbm::constrain(zone.dbm(), zone.dim(), i+1, virt_id+1, tchecker::ineq_cmp_t::LE, 0);
    tchecker::dbm::constrain(zone.dbm(), zone.dim(), virt_id+1, i+1, tchecker::ineq_cmp_t::LE, 0);
  }

  return tchecker::virtual_constraint::factory(zone, vcg->get_no_of_virtual_clocks());
}

void clean_inter_constraint(const std::shared_ptr<tchecker::vcg::vcg_t> vcg, std::string postfix, 
                            std::string & inter_constraint, tchecker::clock_id_t add_to_id)
{
  for(tchecker::clock_id_t i = 0; i < vcg->get_no_of_original_clocks(); ++i) {
    std::string clk_name = vcg->system().clock_name(i);
    clk_name += postfix;
    std::string chi_name = "chi" + std::to_string(i + add_to_id);
    size_t pos = inter_constraint.find(clk_name);
    while(std::string::npos != pos) {
      inter_constraint.replace(pos, clk_name.length(), chi_name);
      pos = inter_constraint.find(clk_name);
    }
  }
}

void apply_inter_constraint(const std::shared_ptr<tchecker::vcg::vcg_t> vcg, std::string & inter_constraint,
                            tchecker::zg::zone_t & zone)
{
  tchecker::clock_constraint_container_t clk_constraints;
  try {
    tchecker::from_string(clk_constraints, vcg->system().clock_variables(), inter_constraint);
    tchecker::dbm::constrain(zone.dbm(), zone.dim(), clk_constraints);
  }
  catch (...) {
    throw std::runtime_error(std::string("problems with inter_constraint: ") + inter_constraint);
  }

}

std::pair<tchecker::zg::state_sptr_t, tchecker::zg::state_sptr_t>
create_initial_states(const std::shared_ptr<tchecker::vcg::vcg_t> vcg_first,
                      const std::shared_ptr<tchecker::vcg::vcg_t> vcg_second,
                      std::map<std::string, std::string> & first_starting_state, std::vector<tchecker::zg::zg_t::sst_t> & sst_first,
                      std::map<std::string, std::string> & second_starting_state, std::vector<tchecker::zg::zg_t::sst_t> & sst_second,
                      std::string & inter_constraint)
{

  init(vcg_first, first_starting_state, sst_first);
  init(vcg_second, second_starting_state, sst_second);

  tchecker::zg::state_sptr_t first{std::get<1>(sst_first[0])};
  tchecker::zg::state_sptr_t second{std::get<1>(sst_second[0])};

  auto first_vc = create_and_apply_vc(vcg_first, first->zone(), 0);
  auto second_vc = create_and_apply_vc(vcg_second, second->zone(), vcg_first->get_no_of_original_clocks());

  tchecker::dbm::constrain(first->zone().dbm(), first->zone().dim(), second_vc->get_vc(vcg_first->get_no_of_original_clocks(), true));
  tchecker::dbm::constrain(second->zone().dbm(), second->zone().dim(), first_vc->get_vc(vcg_second->get_no_of_original_clocks(), true));

  if(vcg_first->get_urgent_or_committed() || vcg_second->get_urgent_or_committed()) {
    tchecker::dbm::reset_to_value(first->zone().dbm(), first->zone().dim(), vcg_first->get_no_of_original_clocks() + vcg_first->get_no_of_virtual_clocks(), 0);
    tchecker::dbm::reset_to_value(second->zone().dbm(), second->zone().dim(), vcg_second->get_no_of_original_clocks() + vcg_second->get_no_of_virtual_clocks(), 0);
  }

  if(!inter_constraint.empty()) {
    clean_inter_constraint(vcg_first, "_1", inter_constraint, 0);
    clean_inter_constraint(vcg_second, "_2", inter_constraint, vcg_first->get_no_of_original_clocks());

    apply_inter_constraint(vcg_first, inter_constraint, first->zone());
    apply_inter_constraint(vcg_second, inter_constraint, second->zone());
  }

  return std::make_pair(first, second);
}

} // end of namespace strong_timed_bisim

} // end of namespace tchecker