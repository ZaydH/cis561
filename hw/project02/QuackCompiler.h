//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <string>
#include <ifstream>

#ifndef PROJECT01_QUACKCOMPILER_H
#define PROJECT01_QUACKCOMPILER_H

class QuackCompiler {

  QuackCompiler() = default;

  void parse_args(int argc, char* argv[]) {
    if (argc == 1) {
      std::cerr << "Insufficient input arguments.  At least one parameter required." << std::flush;
      exit(EXIT_FAILURE);
    }

    int c;
    while ((c = getopt(argc, argv, "t")) != -1) {
      if (c == 't') {
        std::cerr << "Warning: Running in debugging mode" << std::endl;
        debug_ = true;
      }
    }
    // Verify that there is at least one file to parse
    if (optind == argc) {
      std::cerr << "No source files specified for parsing. At least one is required." << std::flush;
      exit(EXIT_FAILURE);
    }
  }

  void run(const std::string & input_file) {
    input_file_ = input_file;
    ifstream f_in(input_file_);
  }

 private:

  /**
   * Select to run the compiler in debug mode.
   */
  bool debug_ = false;
  /**
   * Input file to be compiled.
   */
  std::string input_file_;
};

#endif //PROJECT01_QUACKCOMPILER_H
