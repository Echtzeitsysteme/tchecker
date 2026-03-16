/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <tchecker/publicapi/json_parser.hh>

#if USE_BOOST_JSON
#include <boost/json.hpp>
#endif

namespace tchecker {

namespace publicapi {

#if USE_BOOST_JSON
/*!
 \brief Parse JSON description of state as a map of attributes
 \param state_json : JSON description of state
 \pre state_json is a JSON object with keys: vloc, intval and zone
 \note all other keys in state_json are ignored
 \return state_json vloc, intval and zone as a map of attributes
 \throw std::invalid_argument : if state_json cannot be parsed, or if the precondition is not
 satisfied
*/
std::map<std::string, std::string> parse_state_json(std::string const & state_json)
{
  boost::system::error_code ec;
  boost::json::value json_value = boost::json::parse(state_json, ec);
  if (ec)
    throw std::invalid_argument("Syntax error in JSON state description: " + state_json);

  if (json_value.kind() != boost::json::kind::object)
    throw std::invalid_argument("State description is not a JSON object: " + state_json);

  boost::json::object const & json_obj = json_value.get_object();

  auto value_as_string = [&](boost::json::object const & obj, std::string const & key) {
    boost::json::value const & v = obj.at(key);
    if (v.kind() != boost::json::kind::string)
      throw std::invalid_argument("Unexpected value for key " + key + ", expecting a string");
    return v.get_string();
  };

  std::map<std::string, std::string> attributes;
  attributes["vloc"] = value_as_string(json_obj, "vloc");
  attributes["intval"] = value_as_string(json_obj, "intval");
  if(json_obj.contains("zone")) {
    attributes["zone"] = value_as_string(json_obj, "zone");
  } else {
    attributes["clockval"] = value_as_string(json_obj, "clockval");
  }
  return attributes;
}
#endif

} // end of namespace publicapi

} // end of namespace tchecker