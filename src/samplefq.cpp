#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <boost/program_options.hpp>
#include <omp.h>
#include "io.hpp"
#include "sample.hpp"
#include "git_config.hpp"
#include "verbosity.hpp"

using namespace std;

const string program_name = "samplefq";

void count_seq(istream &is, size_t &cnt) {
  string line;
  while (not is.eof() and getline(is, line))
    if (line[0] == '@') {
      cnt++;
      getline(is, line);
      getline(is, line);
      getline(is, line);
    }
}

void print_out(istream &is, const vector<size_t> &idxs, ostream &os) {
  string line;
  size_t idx = 0;
  auto it = begin(idxs);
  while (it != end(idxs) and getline(is, line))
    if (line[0] == '@') {
      if (idx++ == *it) {
        it++;
        os << line << endl;
        getline(is, line);
        os << line << endl;
        getline(is, line);
        os << line << endl;
        getline(is, line);
        os << line << endl;
      }
    }
}

int main(int argc, char **argv) {
  const string default_error_msg =
      "Please see the command line help with -h or --help.";

  size_t k = 0, n = 0, seed = 0;
  string path1 = "", path2 = "";
  bool single_threaded = false;

  Verbosity verbosity = Verbosity::Info;

  size_t cols = 80;

  namespace po = boost::program_options;

  // Declare the supported options.
  po::options_description desc("Basic options", cols);
  try {
    desc.add_options()
      ("help,h", "produce help message")
      ("version", "Print out the version. Also show git SHA1 with -v.")
      ("verbose,v", "Be verbose about the progress.")
      ("noisy,V", "Be very verbose about the progress.")
      ;

    po::options_description sampling_options("Sampling options", cols);
    sampling_options.add_options()
      ("read1,1", po::value(&path1)->required(),
       "Path to FASTAQ file. [REQUIRED]")
      ("read2,2", po::value(&path2),
       "Path to FASTAQ file. Has to be paired with the first.")
      ("sample,k", po::value(&k)->required(),
       "How many sequences to sample. [REQUIRED]")
      ("size,n", po::value(&n),
       "How many sequences there are in the FASTQ files. "
       "If not specified it will be determined by reading the first FASTQ file prior to sampling.")
      ("seed,s", po::value(&seed),
       "Seed to initialize the pseudo random number generator.")
      ("onethread", po::bool_switch(&single_threaded),
       "Run single threaded. By default, two threads are used when processing paired FASTQ files.");
      ;


    desc.add(sampling_options);
  } catch (...) {
    cout << "Error while generating command line options." << endl
         << "Please notify the developers." << endl;
    return EXIT_FAILURE;
  }

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
  } catch (po::unknown_option &e) {
    cout << "Error while parsing command line options:" << endl << "Option "
         << e.get_option_name() << " not known." << endl << default_error_msg
         << endl;
    return EXIT_FAILURE;
  } catch (po::ambiguous_option &e) {
    cout << "Error while parsing command line options:" << endl << "Option "
         << e.get_option_name() << " is ambiguous." << endl << default_error_msg
         << endl;
    return EXIT_FAILURE;
  } catch (po::multiple_values &e) {
    cout << "Error while parsing command line options:" << endl << "Option "
         << e.get_option_name() << " was specified multiple times." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::multiple_occurrences &e) {
    cout << "Error while parsing command line options:" << endl << "Option --"
         << e.get_option_name() << " was specified multiple times." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::invalid_option_value &e) {
    cout << "Error while parsing command line options:" << endl
         << "The value specified for option " << e.get_option_name()
         << " has an invalid format." << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::too_many_positional_options_error &e) {
    cout << "Error while parsing command line options:" << endl
         << "Too many positional options were specified." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::invalid_command_line_syntax &e) {
    cout << "Error while parsing command line options:" << endl
         << "Invalid command line syntax." << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::invalid_command_line_style &e) {
    cout << "Error while parsing command line options:" << endl
         << "There is a programming error related to command line style."
         << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::reading_file &e) {
    cout << "Error while parsing command line options:" << endl
         << "The configuration file can not be read." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::validation_error &e) {
    cout << "Error while parsing command line options:" << endl
         << "Validation of option " << e.get_option_name() << " failed." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::error &e) {
    cout << "Error while parsing command line options:" << endl
         << "No further information as to the nature of this error is "
            "available, please check your command line arguments." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (exception &e) {
    cout << "An error occurred while parsing command line options." << endl
         << e.what() << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  }

  if (vm.count("verbose")) verbosity = Verbosity::Verbose;
  if (vm.count("noisy")) verbosity = Verbosity::Debug;

  if (vm.count("version") and not vm.count("help")) {
    cout << program_name << " " << GIT_DESCRIPTION << " [" << GIT_BRANCH
         << " branch]" << endl;
    if (verbosity >= Verbosity::Verbose) cout << GIT_SHA1 << endl;
    return EXIT_SUCCESS;
  }

  if (vm.count("help")) {
    cout << program_name << " " << GIT_DESCRIPTION << endl
         << "Copyright (C) 2015 Jonas Maaskola\n"
            "Provided under GNU General Public License Version 3 or later.\n"
            "See the file COPYING provided with this software for details of "
            "the license.\n"
         << endl;
    // cout << limit_line_length(gen_usage_string(), cols) << endl; // TODO
    cout << "This program samples sequences without replacement from one or a pair of FASTQ\n"
            "files. The purpose for doing this is saturation analysis, i.e. to determine\n"
            "whether the performed sequencing has reached saturation.\n"
         << endl;
    cout << "Sequences sampled from the first (or only) FASTQ file are written to the\n"
            "standard output stream, while sequences sampled from the second FASTQ file are\n"
            "written to the standard error stream.\n"
         << endl;
    cout << "Note that the input FASTQ files may also be compressed using gzip or bzip2.\n"
            "Depending on their filename ending (.gz or .bz2), they will be uncompressed on\n"
            "the fly.\n"
         << endl;
    cout << desc << "\n";
    return EXIT_SUCCESS;
  }

  try {
    po::notify(vm);
  } catch (po::multiple_values &e) {
    cout << "Error while parsing command line options:" << endl << "Option "
         << e.get_option_name() << " was specified multiple times." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::invalid_option_value &e) {
    cout << "Error while parsing command line options:" << endl
         << "The value specified for option " << e.get_option_name()
         << " has an invalid format." << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::too_many_positional_options_error &e) {
    cout << "Error while parsing command line options:" << endl
         << "Too many positional options were specified." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::invalid_command_line_syntax &e) {
    cout << "Error while parsing command line options:" << endl
         << "Invalid command line syntax." << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::invalid_command_line_style &e) {
    cout << "Error while parsing command line options:" << endl
         << "There is a programming error related to command line style."
         << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::reading_file &e) {
    cout << "Error while parsing command line options:" << endl
         << "The configuration file can not be read." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::required_option &e) {
    cout << "Error while parsing command line options:" << endl
         << "The required option " << e.get_option_name()
         << " was not specified." << endl << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::validation_error &e) {
    cout << "Error while parsing command line options:" << endl
         << "Validation of option " << e.get_option_name() << " failed." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  } catch (po::error &e) {
    cout << "Error while parsing command line options:" << endl
         << "No further information as to the nature of this error is "
            "available, please check your command line arguments." << endl
         << default_error_msg << endl;
    return EXIT_FAILURE;
  }

  if(n == 0) {
    parse_file(path1, count_seq, n);
    if (verbosity >= Verbosity::Verbose)
      cout << "# There are " << n << " sequences" << endl;
  } else {
    if (verbosity >= Verbosity::Verbose)
      cout << "# Using the first " << n << " sequences" << endl;
  }

  if (k > n) {
    cerr << "Error: cannot sample " << k << " sequence when there are only "
         << n << " sequences in " << path1 << endl;
    return EXIT_FAILURE;
  }

  std::random_device rd;
  std::mt19937 rng(rd());
  if (vm.count("seed"))
    rng.seed(seed);

  vector<size_t> idxs = sample_without_replacement(k, n, rng);

  sort(begin(idxs), end(idxs));

  if (single_threaded)
    omp_set_num_threads(1);

#pragma omp sections
  {
    { parse_file(path1, print_out, idxs, cout); }
#pragma omp section
    {
      if (path2 != "") parse_file(path2, print_out, idxs, cerr);
    }
  }

  return EXIT_SUCCESS;
}
