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
#include <tuple>
#include <unordered_set>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/publicapi/syntax_api.hh"
#include "tchecker/parsing/parsing.hh"


/*!
 \file tck-syntax.cc
 \brief Syntax checking and translation of systems
 */

static struct option long_options[] = {{"asynchronous-events", no_argument, 0, 0},
                                       {"check", no_argument, 0, 'c'},
                                       {"product", no_argument, 0, 'p'},
                                       {"only-processes", no_argument, 0, 0},
                                       {"output", required_argument, 0, 'o'},
                                       {"delimiter", required_argument, 0, 'd'},
                                       {"process-name", required_argument, 0, 'n'},
                                       {"transform", no_argument, 0, 't'},
                                       {"json", no_argument, 0, 'j'},
                                       {"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};

static char * const options = (char *)"cd:hn:o:ptj";

void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   --asynchronous-events  reports all asynchronous events in the model" << std::endl;
  std::cerr << "   -c                     syntax check (timed automaton)" << std::endl;
  std::cerr << "   -p                     synchronized product" << std::endl;
  std::cerr << "   -t                     transform a system into dot graphviz file format" << std::endl;
  std::cerr << "   -j                     transform a system into json file format" << std::endl;
  std::cerr << "   -o file                output file" << std::endl;
  std::cerr << "   -d delim               delimiter string (default: _)" << std::endl;
  std::cerr << "   -n name                name of synchronized process (default: P)" << std::endl;
  std::cerr << "   --only-processes       only output processes in dot graphviz format(combined with -t)" << std::endl;
  std::cerr << "   -h                     help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static bool report_asynchronous_events = false;
static bool check_syntax = false;
static bool synchronized_product = false;
static bool transform = false;
static bool json = false;
static bool only_processes = false;
static bool help = false;
static std::string delimiter = "_";
static std::string process_name = "P";
static std::string output_file = "";

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
      case 'c':
        check_syntax = true;
        break;
      case 'd':
        delimiter = optarg;
        break;
      case 'h':
        help = true;
        break;
      case 'n':
        process_name = optarg;
        break;
      case 'o':
        if (strcmp(optarg, "") == 0)
          throw std::invalid_argument("Invalid empty output file name");
        output_file = optarg;
        break;
      case 'p':
        synchronized_product = true;
        break;
      case 't':
        transform = true;
        break;
      case 'j':
        json = true;
        break;
      default:
        throw std::runtime_error("This should never be executed");
        break;
      }
    }
    else {
      if (strcmp(long_options[long_option_index].name, "asynchronous-events") == 0)
        report_asynchronous_events = true;
      else if (strcmp(long_options[long_option_index].name, "only-processes") == 0)
        only_processes = true;
      else
        throw std::runtime_error("This also should never be executed");
    }
  }

  return optind;
}

/*!
 \brief Load system from a file
 \param filename : file name
 \return The system declaration loaded from filename, nullptr if parsing error occurred
*/
std::shared_ptr<tchecker::parsing::system_declaration_t> load_system(std::string const & filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }

  if (sysdecl == nullptr)
    tchecker::log_output_count(std::cout);

  return sysdecl;
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

    if (synchronized_product && transform) {
      std::cerr << "Command line options -p and -t are incompatible" << std::endl;
      usage(argv[0]);
      return EXIT_FAILURE;
    }

    if (help) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);

    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system(input_file)};
    if (sysdecl.get() == nullptr)
      return EXIT_FAILURE;

    if (check_syntax)
      tchecker::publicapi::tck_syntax_check_syntax(output_file, input_file);

    if (report_asynchronous_events)
      tchecker::publicapi::do_report_asynchronous_events(input_file);

    if (synchronized_product)
      tchecker::publicapi::tck_syntax_create_synchronized_product(output_file, input_file, process_name, delimiter);

    if (transform)
      tchecker::publicapi::tck_syntax_to_dot(output_file, input_file, delimiter);
    else if (json)
      tchecker::publicapi::tck_syntax_to_json(output_file, input_file, delimiter);
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
