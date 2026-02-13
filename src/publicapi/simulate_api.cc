/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <tchecker/publicapi/simulate_api.hh>

#include <fstream>
#include <iostream>
#include <map>

#if USE_BOOST_JSON
#include <boost/json.hpp>
#endif

#include <tchecker/simulate/simulate.hh>

void tck_simulate(const char * output_filename, const char * sysdecl_filename, const simulation_type_t simulation_type,
                  const tchecker::simulate::display_type_t display_type, const char * starting_state_json, const int * nsteps,
                  const bool output_trace)
{
  int nsteps_value = 0;
  if(nullptr != nsteps) {
    nsteps_value = *nsteps;
  }

  tchecker::publicapi::tck_simulate(output_filename, sysdecl_filename, simulation_type, display_type, starting_state_json,
                                    nsteps_value, output_trace);
}

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
  attributes["zone"] = value_as_string(json_obj, "zone");
  return attributes;
}
#endif

void tck_simulate(std::string output_filename, std::string sysdecl_filename, simulation_type_t simulation_type,
                  tchecker::simulate::display_type_t display_type, std::string starting_state_json, std::size_t nsteps,
                  bool output_trace)
{
  try {
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
    sysdecl = tchecker::parsing::parse_system_declaration(sysdecl_filename);
    if (sysdecl == nullptr) {
      throw std::runtime_error("nullptr system declaration");
    }
    std::shared_ptr<tchecker::system::system_t> system = std::make_shared<tchecker::system::system_t>(*sysdecl);

#if !USE_BOOST_JSON
    if (display_type != tchecker::simulate::HUMAN_READABLE_DISPLAY) {
      std::cerr << "JSON display is not enabled in this build" << std::endl;
      return;
    }
#endif

    std::ostream * os = nullptr;
    std::ofstream ofs;

    if (output_filename != "") {
      ofs.open(output_filename, std::ios::out);
      if (!ofs) {
        throw std::runtime_error("Failed to open file: " + output_filename);
      }
      os = &ofs;
    }
    else {
      os = &std::cout;
    }

    std::map<std::string, std::string> starting_state_attributes;
#if USE_BOOST_JSON
    if (!starting_state_json.empty()) {
      starting_state_attributes = parse_state_json(starting_state_json);
    }
#endif

    std::shared_ptr<tchecker::simulate::state_space_t> state_space{nullptr};
    if (simulation_type == INTERACTIVE_SIMULATION)
      state_space = tchecker::simulate::interactive_simulation(*sysdecl, display_type, *os, starting_state_attributes);
    else if (simulation_type == RANDOMIZED_SIMULATION)
      state_space = tchecker::simulate::randomized_simulation(*sysdecl, nsteps, starting_state_attributes);
    else if (simulation_type == ONESTEP_SIMULATION)
      tchecker::simulate::onestep_simulation(*sysdecl, display_type, *os, starting_state_attributes);
    else if (simulation_type == CONCRETE_SIMULATION)
      state_space = tchecker::simulate::concrete_simulation(*sysdecl, display_type, *os);
    else
      throw std::runtime_error("Select one of interactive, one-step, randomized, or concrete simulation");

    if (output_trace) {
      assert(state_space.get() != nullptr);
      tchecker::simulate::dot_output(*os, state_space->graph(), sysdecl->name());
    }
  }
  catch (std::runtime_error & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }
  catch (...) {
    std::cerr << tchecker::log_error << "Unknown error" << std::endl;
  }
}

} // namespace publicapi

} // namespace tchecker
