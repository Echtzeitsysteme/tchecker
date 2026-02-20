/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "tchecker/publicapi/simulate_api.hh"

/*!
 \file tck-simulate.cc
 \brief Command-line simulator for TChecker timed automata models
 */

static struct option long_options[] = {{"interactive", no_argument, 0, 'i'},
                                       {"random", required_argument, 0, 'r'},
                                       {"onestep", no_argument, 0, '1'},
                                       {"concrete", no_argument, 0, 'c'},
                                       {"output", required_argument, 0, 'o'},
                                       {"trace", no_argument, 0, 't'},
                                       {"help", no_argument, 0, 'h'},
#if USE_BOOST_JSON
                                       {"state", required_argument, 0, 's'},
                                       {"json", no_argument, 0, 0},
#endif
                                       {0, 0, 0, 0}};

static char * const options = (char *)"1icr:ho:s:t";

/*!
\brief Print usage message for program progname
*/
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -1          one-step simulation (output initial or next states if combined with -s)" << std::endl;
  std::cerr << "   -i          interactive simulation (default)" << std::endl;
  std::cerr << "   -r N        randomized simulation, N steps" << std::endl;
  std::cerr << "   -c          concrete simulation (can be used in combination with -1)" << std::endl;
  std::cerr << "   -o file     output file for simulation trace (default: stdout)" << std::endl;
#if USE_BOOST_JSON
  std::cerr << "   --json      display states/transitions in JSON format" << std::endl;
  std::cerr << "   -s state    starting state, specified as a JSON object with keys vloc, intval and zone" << std::endl;
  std::cerr << "               vloc: comma-separated list of location names (one per process), in-between < and >" << std::endl;
  std::cerr << "               intval: comma-separated list of assignments (one per integer variable)" << std::endl;
  std::cerr << "               zone: conjunction of clock-constraints (following TChecker expression syntax)" << std::endl;
#endif
  std::cerr << "   -t          output simulation trace, incompatible with -1" << std::endl;
  std::cerr << "   -h          help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static enum simulation_type_t simulation_type = INTERACTIVE_SIMULATION; // DO NOT CHANGE THIS!
static enum tchecker::simulate::display_type_t display_type = tchecker::simulate::HUMAN_READABLE_DISPLAY;
static bool help = false;
static std::size_t nsteps = 0;
static std::string output_filename = "";
static std::string starting_state_json = "";
static bool output_trace = false;

/*!
\brief Parse command line arguments
\param argc : number of command line arguments
\param argv : array of command line arguments
\post Global variables have been set according to argv
*/
int parse_command_line(int argc, char * argv[])
{
  while (true) {
    int long_option_index = -1;
    int c = getopt_long(argc, argv, options, long_options, &long_option_index);

    if (c == -1)
      break;

    if (c == ':')
      throw std::runtime_error("Missing option parameter");
    else if (c == '?')
      throw std::runtime_error("Unknown command-line option");
    else if (c != 0) {
      switch (c) {
      case '1':
        if(simulation_type == CONCRETE_SIMULATION) {
          simulation_type = CONCRETE_ONESTEP_SIMULATION;
        } else {
          simulation_type = ONESTEP_SIMULATION;
        }
        break;
      case 'i':
        simulation_type = INTERACTIVE_SIMULATION;
        break;
      case 'r': {
        simulation_type = RANDOMIZED_SIMULATION;
        long long int l = std::strtoll(optarg, nullptr, 10);
        if (l < 0)
          throw std::runtime_error("Invalid trace length (must be positive)");
        nsteps = l;
        break;
      }
      case 'c':
        if(simulation_type == ONESTEP_SIMULATION) {
          simulation_type = CONCRETE_ONESTEP_SIMULATION;
        } else {
          simulation_type = CONCRETE_SIMULATION;
        }
        break;
      case 's':
        starting_state_json = optarg;
        break;
      case 't':
        output_trace = true;
        break;
      case 'h':
        help = true;
        break;
      case 'o':
        if (strcmp(optarg, "") == 0)
          throw std::invalid_argument("Invalid empty output file name");
        output_filename = optarg;
        break;
      default:
        throw std::runtime_error("I should never be executed");
        break;
      }
    }
    else {
#if USE_BOOST_JSON
      if (strcmp(long_options[long_option_index].name, "json") == 0)
        display_type = tchecker::simulate::JSON_DISPLAY;
      else
#endif
        throw std::runtime_error("This also should never be executed");
    }
  }

  return optind;
}

/*!
 \brief Main function
*/
int main(int argc, char * argv[])
{
  try {
    int optindex = parse_command_line(argc, argv);

    if (argc - optindex > 1) {
      std::cerr << "Too many input files" << std::endl;
      usage(argv[0]);
      return EXIT_FAILURE;
    }

    if (help) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    }

    if (output_trace && (simulation_type == ONESTEP_SIMULATION)) {
      std::cerr << "Cannot output trace in one-step simulation" << std::endl;
      return EXIT_FAILURE;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);
    if (input_file == "" || input_file == "-")
      std::cerr << "Reading model from standard input" << std::endl;

    tchecker::publicapi::tck_simulate(output_filename, input_file, simulation_type, display_type, starting_state_json, nsteps,
                                      output_trace);

    if (tchecker::log_error_count() > 0)
      return EXIT_FAILURE;
    else {
      return EXIT_SUCCESS;
    }
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
