/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */


#include <fstream>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <string.h>


#include "tchecker/publicapi/compare_api.hh"
#include "tchecker/utils/log.hh"

/*
 \file tck-compare.cc
 \brief Comparison of timed automata
 */

static struct option long_options[] = {{"relationship", required_argument, 0, 'r'},
                                       {"output", required_argument, 0, 'o'},
                                       {"help", no_argument, 0, 'h'},
                                       {"block-size", required_argument, 0, 0},
                                       {"table-size", required_argument, 0, 0},
                                       {0, 0, 0, 0}};

static char const * const options = (char *)"hr:n:";

/*!
  \brief Display usage
  \param progname : programme name
 */
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file1] [file2]" << std::endl;
  std::cerr << "   -h                 help" << std::endl;
  std::cerr << "   -o out_file        output file for certificate (default is standard output)" << std::endl;
  std::cerr << "   -r relationship    relationship to check" << std::endl;
  std::cerr << "                strong-timed-bisim  strong timed bisimilarity" << std::endl;
}

enum tck_compare_relationship_t relationship = STRONG_TIMED_BISIM;   /*!< Selected relationship */
bool help = false;                                 /*!< Help flag */
std::string output_file = "";                      /*!< Output file name (empty means standard output) */
std::ostream * os = &std::cout;                    /*!< Default output stream */
std::size_t block_size = 10000;                    /*!< Size of allocated blocks */
std::size_t table_size = 65536;                    /*!< Size of hash tables */

/*!
 \brief Parse command-line arguments
 \param argc : number of arguments
 \param argv : array of arguments
 \pre argv[0] up to argv[argc-1] are valid accesses
 \post global variables help, output_file, and labels have been set
 from argv
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
      case 'r':
        if (strcmp(optarg, "strong-timed-bisim") == 0)
          relationship = STRONG_TIMED_BISIM;
        else
          throw std::runtime_error("Unknow relationship: " + std::string(optarg));
        break;
      case 'o':
        output_file = optarg;
        break;
      case 'h':
        help = true;
        break;
      default:
        throw std::runtime_error("This should never be executed");
        break;
      }
    }
    else {
      if (strcmp(long_options[long_option_index].name, "block-size") == 0)
        block_size = std::strtoull(optarg, nullptr, 10);
      else if (strcmp(long_options[long_option_index].name, "table-size") == 0)
        table_size = std::strtoull(optarg, nullptr, 10);
      else
        throw std::runtime_error("This also should never be executed");
    }
  }

  return optind;
}

/*!
 \brief Main function
 */
int main(int argc, char * argv[]) {
  try{
    int optindex = parse_command_line(argc, argv);
    if (argc - optindex != 2) {
      std::cerr << "exactly two input files needed" << std::endl;
      usage(argv[0]);
      return EXIT_FAILURE;
    }

    if (help) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    }

    std::string first_input = argv[argc - 2];
    std::string second_input = argv[argc - 1];

    
    std::shared_ptr<std::ofstream> os_ptr{nullptr};

    tchecker::publicapi::tck_compare(output_file, first_input, second_input, relationship, block_size, table_size);

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














