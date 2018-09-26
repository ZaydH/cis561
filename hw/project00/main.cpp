#include <iostream>

#include "html_parser.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Invalid input argument count.  Only a single argument allowed.";
    exit(EXIT_FAILURE);
  }

  HtmlParser parser = HtmlParser(std::string(argv[1]));
  parser.run();

  return 0;
}
