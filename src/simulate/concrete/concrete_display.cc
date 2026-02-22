/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */
#include "tchecker/config.hh"
#if USE_BOOST_JSON
#include <boost/json.hpp>
#endif

#include "tchecker/simulate/concrete/concrete_display.hh"
#include "tchecker/simulate/concrete/clock_names.hh"
#include "tchecker/operational-semantics/zone_valuation_converter.hh"

namespace tchecker {

namespace simulate {

namespace concrete {

/* concrete_display_t*/

concrete_display_t::concrete_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg)
      : _os(os), _zg(zg)
{
  assert(nullptr != _zg.get());
}

std::map<std::string, std::string> 
concrete_display_t::gen_attr_map(tchecker::zg::const_state_sptr_t const & s, std::map<std::string, std::string> & attr)
{
  assert(tchecker::dbm::is_tight(s->zone().dbm(), s->zone().dim()));
  assert(s->zone().dim() == _zg->clocks_count() + 1);

  _zg->attributes(s, attr);
  attr.erase("zone");

  std::shared_ptr<tchecker::clockval_t> valuation = tchecker::operational_semantics::convert(s->zone());

  std::string clockval_str = to_string(*valuation, clock_names(_zg->system()));

  if (clockval_str.rfind("Ref Clock=0,", 0) == 0) { 
    clockval_str.erase(0, std::string("Ref Clock=0,").length());
  }

  attr["clockval"] = clockval_str;
  return attr;
}

/* concrete_hr_display_t*/

concrete_hr_display_t::concrete_hr_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg) : concrete_display_t(os, zg)
{
}

void concrete_hr_display_t::output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v, 
                                        bool finite_max_delay, tchecker::clock_rational_value_t max_delay)
{
  _os << "--- Current state: " << std::endl;
  output_state(s);
  _os << "--- Successors: " << std::endl;

  std::size_t i = 0;
  for (auto && [status, nexts, nextt] : v) {
    _os << i << ") ";
    output(tchecker::zg::const_transition_sptr_t{nextt});
    _os << std::endl;
    output_state(tchecker::zg::const_state_sptr_t{nexts});
    ++i;
  }

  std::ostringstream oss;
  oss << max_delay;

  _os << "--- Max Delay: " << ((finite_max_delay) ? oss.str() : std::string("infinite")) << std::endl;
   
}

void concrete_hr_display_t::output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  _os << "--- Initial states: " << std::endl;

  std::size_t i = 0;
  for (auto && [status, nexts, nextt] : v) {
    _os << i << ") ";
    output_state(tchecker::zg::const_state_sptr_t{nexts});
    ++i;
  }
}

void concrete_hr_display_t::output_state(tchecker::zg::const_state_sptr_t const & s)
{
  
  std::map<std::string, std::string> attr;
  concrete_display_t::gen_attr_map(s, attr);

  for (auto && [key, value] : attr)
    _os << "\t" << key << ": " << value << std::endl;
}

void concrete_hr_display_t::output(tchecker::zg::const_transition_sptr_t const & t)
{
  std::map<std::string, std::string> attr;
  _zg->attributes(t, attr);
  for (auto && [key, value] : attr)
    _os << "\t" << key << ": " << value << " ";
}


/* concrete_json_display_t */
#if USE_BOOST_JSON

concrete_json_display_t::concrete_json_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg) : concrete_display_t(os, zg)
{
}

void concrete_json_display_t::output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v, 
                                          bool finite_max_delay, tchecker::clock_rational_value_t max_delay) {
  boost::json::array a;

  for (auto && [status, nexts, nextt] : v) {
    boost::json::object o;
    o.emplace("transition", output(tchecker::zg::const_transition_sptr_t{nextt}));
    o.emplace("target", output(tchecker::zg::const_state_sptr_t{nexts}));
    a.emplace_back(o);
  }

  boost::json::object o;
  o.emplace("current", output(s));
  o.emplace("next", a);
  std::string m_d = std::to_string(max_delay.numerator()) + ((1 != max_delay.denominator()) ? ("/" + max_delay.denominator()) : "");
  o.emplace("max_delay", (finite_max_delay) ? m_d : std::string("infinite"));
  _os << o << std::endl;
}

void concrete_json_display_t::output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  boost::json::array a;
  for (tchecker::zg::zg_t::sst_t const & sst : v) {
    boost::json::object o;
    auto && [status, s, t] = sst;
    o.emplace("status", status);
    o.emplace("state", output(tchecker::zg::const_state_sptr_t{s}));
    o.emplace("transition", output(tchecker::zg::const_transition_sptr_t{t}));
    a.emplace_back(o);
  }

  boost::json::object o;
  o.emplace("initial", a);

  _os << o << std::endl;
}

void concrete_json_display_t::output_state(tchecker::zg::const_state_sptr_t const & s)
{
  _os << output(s) << std::endl;
}

boost::json::value concrete_json_display_t::output(tchecker::zg::const_state_sptr_t const & s)
{
  std::map<std::string, std::string> attr;
  concrete_display_t::gen_attr_map(s, attr);
  
  return boost::json::value_from(attr);
}

boost::json::value concrete_json_display_t::output(tchecker::zg::const_transition_sptr_t const & t)
{
  std::map<std::string, std::string> attr;
  _zg->attributes(t, attr);
  return boost::json::value_from(attr);
}

#endif

/* factory */

concrete_display_t * concrete_display_factory(enum tchecker::simulate::display_type_t display_type, std::ostream & os,
                                              std::shared_ptr<tchecker::zg::zg_t> zg)
{
  switch (display_type) {
  case tchecker::simulate::HUMAN_READABLE_DISPLAY:
    return new concrete_hr_display_t{os, zg};
#if USE_BOOST_JSON
  case tchecker::simulate::JSON_DISPLAY:
    return new concrete_json_display_t{os, zg};
#endif
  default:
    throw std::runtime_error("This should never occur: switch statement is not complete");
  }
}

} // end of namespace concrete

} // end of namespace simulate

} // end of namespace tchecker
