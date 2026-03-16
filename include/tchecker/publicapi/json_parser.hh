/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

/*!
  \file json_parser.hh
  \brief Function to path initial states given as json
*/

#ifndef TCHECKER_PUBLICAPI_JSON_PARSER_HH
#define TCHECKER_PUBLICAPI_JSON_PARSER_HH

#include<map>
#include<string>

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
std::map<std::string, std::string> parse_state_json(std::string const & state_json);

#endif

} // end of namespace publicapi

} // end of namespace tchecker
#endif