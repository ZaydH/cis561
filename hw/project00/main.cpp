#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Invalid input argument count.  Only a single argument allowed."
    exit(EXIT_FAILURE);
  }

  return 0;
}
