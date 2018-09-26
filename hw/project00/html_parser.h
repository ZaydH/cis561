//
// Created by Zayd Hammoudeh on 9/25/18.
//

#ifndef PROJECT00_HTML_PARSER_H
#define PROJECT00_HTML_PARSER_H

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "lex.yy.h"
#include "tree_node.h"

class HtmlParser {
 public:
  explicit HtmlParser(const std::string &input_path) : input_file_path_(input_path) {

    // Define the output file path
    std::string::size_type per_loc = input_file_path_.rfind('.');
    std::stringstream ss;
    std::string output_label = "_out";
    if (per_loc == std::string::npos) {
      ss << input_path << output_label;
    } else {
      ss << input_file_path_.substr(0, (int)per_loc)
         << output_label
         << input_file_path_.substr(per_loc + 1, std::string::npos);
    }
    output_file_path_ = ss.str();
  };

  void run() {
    std::ifstream fin(input_file_path_);
    if (!fin) {
      std::cerr << "No file found: " << input_file_path_ << ". Exiting...";
      exit(EXIT_FAILURE);
    }

//    yy::Lexer lexer(fin);
    yy::Lexer lexer(std::cin);
    ASTNode *root;
    auto *parser = new yy::parser(lexer, &root);

    // Check if the parsing passed and if so write the output file
    if (!root) {
      std::cerr << "Error parsing file: " << input_file_path_;
    } else {
      std::cout << "File parse successful.";
      std::ofstream fout(output_file_path_);
      root->PRINT(fout);
      fout.close();
    }
    delete parser;
    delete root;
    fin.close();
  }


 private:
  std::string input_file_path_;
  std::string output_file_path_;
};

#endif //PROJECT00_HTML_PARSER_H
