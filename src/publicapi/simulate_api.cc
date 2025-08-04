#include <fstream>
#include <iostream>
#include <map>

#if USE_BOOST_JSON
#include <boost/json.hpp>
#endif

#include <tchecker/simulate/simulate.hh>

#include <tchecker/publicapi/simulate_api.hh>

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
  attributes["zone"] = value_as_string(json_obj, "zone");
  return attributes;
}
#endif


const void tck_simulate_onestep_simulation(const char * output_filename, const char * sysdecl_filename,
                                           const tchecker::simulate::display_type_t display_type,
                                           const char * starting_state_json)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);

  if (sysdecl == nullptr) {
    tchecker::log_output_count(std::cout);
    return;
  }

  if (display_type != tchecker::simulate::HUMAN_READABLE_DISPLAY) {
#if !USE_BOOST_JSON
    std::cerr << "JSON display is not enabled in this build" << std::endl;
    return;
#endif
  }

  std::ostream * os = nullptr;
  if (output_filename && std::strlen(output_filename) > 0) {
    os = new std::ofstream(output_filename, std::ios::out);
  }
  else
    os = &std::cout;

  std::map<std::string, std::string> starting_state_attributes;
#if USE_BOOST_JSON
  if (starting_state_json != nullptr && std::strlen(starting_state_json) > 0) {
    starting_state_attributes = parse_state_json(starting_state_json);
  }
#endif



    tchecker::simulate::onestep_simulation(*sysdecl, display_type, *os, starting_state_attributes);
    
    if (os != &std::cout)
        delete os;
}
