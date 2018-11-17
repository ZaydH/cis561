//
// Created by Zayd Hammoudeh on 11/16/18.
//

#ifndef TYPE_CHECKER_CODE_GEN_UTILS_H
#define TYPE_CHECKER_CODE_GEN_UTILS_H

#include <fstream>

namespace CodeGen {
  struct Settings {
    std::ofstream & fout_;

    explicit Settings(std::ofstream& fout) : fout_(fout) {}
  };
}

#endif //TYPE_CHECKER_CODE_GEN_UTILS_H
