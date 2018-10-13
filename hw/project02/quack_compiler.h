//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <string>
#include <fstream>
#include <iostream>

#include "quack_program.h"
#include "lex.yy.h"
#include "quack_classes.h"

#ifndef PROJECT01_QUACKCOMPILER_H
#define PROJECT01_QUACKCOMPILER_H

namespace Quack {
  class Compiler {
   public:
    Compiler() {
      initialize();
    }

    ~Compiler() {
      for (const auto &prog : progs_)
        delete prog;
    }
    /**
     * Parse the input command line arguments and configure the compiler.
     *
     * @param argc Number of input arguments
     * @param argv List of input arguments
     */
    void parse_args(unsigned int argc, char *argv[]) {
      if (argc == 1) {
        std::cerr << "Insufficient input arguments.  At least one parameter required."
                  << std::flush;
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
      unsigned int num_files = argc - optind;
      if (num_files == 0) {
        std::cerr << "No source files specified for parsing. At least one is required."
                  << std::flush;
        exit(EXIT_FAILURE);
      }

      input_files_.reserve(num_files);
      progs_.reserve(num_files);
      for (unsigned int i = 0; i < num_files; i++)
        input_files_.emplace_back(argv[i + optind]);
    }

    void run() {
      num_errs_ = 0;
      for (const std::string &file_path : input_files_) {
        std::ifstream f_in(file_path);
        progs_.emplace_back(nullptr);

        // If specified file does not exist, report an error then continue
        if (!f_in) {
          std::cerr << "Unable to locate input file: " << file_path << std::endl;
          num_errs_++;
          continue;
        }

        parse(f_in);
        f_in.close();
      }
    }

    void initialize() {
      Class::Container* classes = Class::Container::singleton();

      if (classes->empty()) {
        classes->add(new ObjectClass());
        classes->add(new IntClass());
        classes->add(new StringClass());
        classes->add(new BooleanClass());
      }
    }

   private:

    void parse(std::istream &f_in) {
      yy::Lexer lexer(f_in);
      Quack::Program* prog;
      yy::parser *parser = new yy::parser(lexer, &prog);

      if (parser->parse() != 0 && report::ok()) {
        std::cout << "Parse failed, no tree\n";
      } else {
        if (debug_)
          prog->print_original_src();
      }
      delete parser;
    }
    /**
     * Select to run the compiler in debug mode.
     */
    bool debug_ = false;
    /**
     * Input file to be compiled.
     */
    std::vector<std::string> input_files_;
    std::vector<Quack::Program*> progs_;

    unsigned int num_errs_ = 0;
  };
}

#endif //PROJECT01_QUACKCOMPILER_H
