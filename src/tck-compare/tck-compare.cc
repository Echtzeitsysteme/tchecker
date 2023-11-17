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


#include "vcg-timed-bisim.hh"

/*
 \file tck-compare.cc
 \brief Comparison of timed automata
 */

static struct option long_options[] = {{"relationship", required_argument, 0, 'r'},
                                       {"order", required_argument, 0, 'n'},
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
  std::cerr << "   -r relationship    relationship to check" << std::endl;
  std::cerr << "                strong-timed-bisim  strong timed bisimilarity" << std::endl;
  std::cerr << "   -n order           the order in which the relationship shall be checked" << std::endl
            << std::endl;
  std::cerr << "   -h                 help" << std::endl;
  std::cerr << "   -o out_file        output file for certificate (default is standard output)" << std::endl;
}

enum relationship_t {
  STRONG_TIMED_BISIM, /*!< Strong Timed Bisimilarity */
};

enum relationship_t relationship = STRONG_TIMED_BISIM;   /*!< Selected relationship */
unsigned int order = std::numeric_limits<unsigned int>::max();
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
 \post global variables help, output_file, search_order and labels have been set
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
      case 'n':
        order = std::stoul(optarg);
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
 \brief Load a system declaration from a file
 \param filename : file name
 \return pointer to a system declaration loaded from filename, nullptr in case
 of errors
 \post all errors have been reported to std::cerr
 */
tchecker::parsing::system_declaration_t * load_system_declaration(std::string const & filename)
{
  tchecker::parsing::system_declaration_t * sysdecl = nullptr;
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
    if (sysdecl == nullptr)
      throw std::runtime_error("nullptr system declaration");
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }
  return sysdecl;
}


/*!
 \brief Perform strong timed bisimilarity check
 \param sysdecl_first : system declaration of the first TA
 \param sysdecl_first : system declaration of the second TA
 \post statistics on strong timed bisimilarity analysis of the system declared by sysdecl have been output to standard output.
 */
void strong_timed_bisim(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_first, std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl_second)
{
  auto stats = tchecker::tck_compare::vcg_timed_bisim::run(sysdecl_first, sysdecl_second, os, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;
}

/*
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
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl_first_TA{load_system_declaration(first_input)};

    std::string second_input = argv[argc - 1];
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl_second_TA{load_system_declaration(second_input)};

    if (tchecker::log_error_count() > 0)
      return EXIT_FAILURE;

    std::shared_ptr<std::ofstream> os_ptr{nullptr};

    if (output_file != "") {
      try {
        os_ptr = std::make_shared<std::ofstream>(output_file);
        os = os_ptr.get();
      }
      catch (std::exception & e) {
        std::cerr << tchecker::log_error << e.what() << std::endl;
        return EXIT_FAILURE;
      }
    }

    if(STRONG_TIMED_BISIM == relationship) {
      strong_timed_bisim(sysdecl_first_TA, sysdecl_second_TA);
    } else {
      throw std::runtime_error("Unknown relationship");
    }

  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}














