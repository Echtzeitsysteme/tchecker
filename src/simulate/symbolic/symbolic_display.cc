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

#include "tchecker/simulate/symbolic/symbolic_display.hh"

namespace tchecker {

namespace simulate {

namespace symbolic {

hr_display_t::hr_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg) : _os(os), _zg(zg)
{
  if (_zg.get() == nullptr)
    throw std::invalid_argument("Invalid nullptr zone graph");
}

void hr_display_t::output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  _os << "--- Initial states: " << std::endl;

  std::size_t i = 0;
  for (auto && [status, nexts, nextt] : v) {
    _os << i << ") ";
    output_state(tchecker::zg::const_state_sptr_t{nexts});
    ++i;
  }
}

void hr_display_t::output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v)
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
}

void hr_display_t::output_state(tchecker::zg::const_state_sptr_t const & s)
{
  std::map<std::string, std::string> attr;
  _zg->attributes(s, attr);
  for (auto && [key, value] : attr)
    _os << "\t" << key << ": " << value << std::endl;
}

void hr_display_t::output(tchecker::zg::const_transition_sptr_t const & t)
{
  std::map<std::string, std::string> attr;
  _zg->attributes(t, attr);
  for (auto && [key, value] : attr)
    _os << "\t" << key << ": " << value << " ";
}


/* json_display_t */
#if USE_BOOST_JSON
/*!
 \brief JSON representation of a state
 \param zg : zone graph
 \param s : state
 \return JSON representation of s from its attributes in zg
*/
static boost::json::value json_state(tchecker::zg::zg_t & zg, tchecker::zg::const_state_sptr_t const & s)
{
  std::map<std::string, std::string> attr;
  zg.attributes(s, attr);
  return boost::json::value_from(attr);
}

/*!
 \brief JSON representation of a transition
 \param zg : zone graph
 \param s : state
 \return JSON representation of t from its attributes in zg
 */
static boost::json::value json_transition(tchecker::zg::zg_t & zg, tchecker::zg::const_transition_sptr_t const & t)
{
  std::map<std::string, std::string> attr;
  zg.attributes(t, attr);
  return boost::json::value_from(attr);
}

/*!
 \brief JSON representation of a triple (status, state, transition)
 \param zg : zone graph
 \param sst : triple (status, state, transition)
 \return JSON representation of t from its attributes in zg
 */
static boost::json::value json_sst(tchecker::zg::zg_t & zg, tchecker::zg::zg_t::sst_t const & sst)
{
  boost::json::object o;
  auto && [status, s, t] = sst;
  o.emplace("status", status);
  o.emplace("state", json_state(zg, tchecker::zg::const_state_sptr_t{s}));
  o.emplace("transition", json_transition(zg, tchecker::zg::const_transition_sptr_t{t}));
  return o;
}

/*!
 \brief JSON representation of a vector of triples (status, state, transition)
 \param zg : zone graph
 \param v : vector of triples (status, state, transition)
 \return JSON object of v from status, attributes of states and attributes of transitions in v
 */
static boost::json::value json_vsst(tchecker::zg::zg_t & zg, std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  boost::json::array a;
  for (tchecker::zg::zg_t::sst_t const & sst : v)
    a.emplace_back(json_sst(zg, sst));
  return a;
}

/*!
 \brief JSON representation of initial states and transitions
 \param zg : zone graph
 \param v : vector of initial states and transitions
 \return JSON object of initial vector v in zg
 */
static boost::json::value json_initial(tchecker::zg::zg_t & zg, std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  boost::json::object o;
  o.emplace("initial", json_vsst(zg, v));
  return o;
}

/*!
 \brief JSON representation of next states and transitions
 \param zg : zone graph
 \param s : state
 \param v : vector of next states and transitions of s
 \return JSON object of s and v in zg
 */
static boost::json::value json_next(tchecker::zg::zg_t & zg, tchecker::zg::const_state_sptr_t const & s,
                                    std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  boost::json::object o;
  o.emplace("current", json_state(zg, s));
  o.emplace("next", json_vsst(zg, v));
  return o;
}

json_display_t::json_display_t(std::ostream & os, std::shared_ptr<tchecker::zg::zg_t> const & zg) : _os(os), _zg(zg)
{
  if (_zg.get() == nullptr)
    throw std::invalid_argument("Invalid nullptr zone graph");
}

void json_display_t::output_initial(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  _os << json_initial(*_zg, v) << std::endl;
}

void json_display_t::output_next(tchecker::zg::const_state_sptr_t const & s, std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  _os << json_next(*_zg, s, v) << std::endl;
}

void json_display_t::output_state(tchecker::zg::const_state_sptr_t const & s)
{
  _os << json_state(*_zg, s) << std::endl;
}

#endif

/* factory */

display_t * display_factory(enum tchecker::simulate::display_type_t display_type, std::ostream & os,
                            std::shared_ptr<tchecker::zg::zg_t> zg)
{
  switch (display_type) {
  case tchecker::simulate::HUMAN_READABLE_DISPLAY:
    return new hr_display_t{os, zg};
#if USE_BOOST_JSON
  case tchecker::simulate::JSON_DISPLAY:
    return new json_display_t{os, zg};
#endif
  default:
    throw std::runtime_error("This should never occur: switch statement is not complete");
  }
}

} // end of namespace symbolic

} // end of namespace simulate

} // end of namespace tchecker