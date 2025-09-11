/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/strong-timed-bisim/certificate/clock_names.hh"

namespace tchecker {

namespace strong_timed_bisim {

namespace certificate {

std::function<std::string (tchecker::clock_id_t)> 
clock_names(const std::shared_ptr<tchecker::vcg::vcg_t> vcg1, const std::shared_ptr<tchecker::vcg::vcg_t> vcg2)
{
  tchecker::clock_id_t no_orig_clks1 = vcg1->get_no_of_original_clocks();
  tchecker::clock_id_t no_orig_clks2 = vcg2->get_no_of_original_clocks();

  tchecker::clock_id_t urgent_clk = (vcg1->get_no_of_original_clocks() + vcg2->get_no_of_original_clocks() < vcg1->get_no_of_virtual_clocks()) ? (1) : (0);

  std::function<std::string (tchecker::clock_id_t)> result =
    [no_orig_clks1, no_orig_clks2, vcg1, vcg2, urgent_clk](tchecker::clock_id_t id) {
      if (id <= (no_orig_clks1 + urgent_clk)) {
        return clock_names(vcg1, "_1")(id);
      }

      id -= (no_orig_clks1 + urgent_clk + 1);
      return clock_names(vcg2, "_2")(id);
    };

  return result;
}

std::function<std::string (tchecker::clock_id_t)> 
clock_names(const std::shared_ptr<tchecker::vcg::vcg_t> vcg, std::string postfix)
{
  tchecker::clock_id_t no_orig_clks = vcg->get_no_of_original_clocks();

  std::function<std::string (tchecker::clock_id_t)> result =
    [no_orig_clks, vcg, postfix](tchecker::clock_id_t id) {
      if(id == 0) {
        return std::string("Ref Clock");
      }
      if (id <= no_orig_clks) {
        return vcg->system().clock_name(id - 1) + postfix;
      }

      if (id == (no_orig_clks + 1)) {
        return std::string("Urgent_Clock");
      }
      throw std::runtime_error(std::string(__FILE__) + std::string(": ") + std::to_string(__LINE__) + std::string(": ") +
                             std::string("strange clock id: ") + std::to_string(id));
    };

    return result;
}

} // end of namespace certificate

} // end of namespace strong_timed_bisim

} // end of namespace tchecker