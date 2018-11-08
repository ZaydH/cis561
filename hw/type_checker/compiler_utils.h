//
// Created by Zayd Hammoudeh on 11/7/18.
//

#ifndef PROJECT02_COMPILER_UTILS_H
#define PROJECT02_COMPILER_UTILS_H

#include <string>

#include "keywords.h"
#include "exceptions.h"

namespace Compiler {

  class Utils {
    static const std::string binop_lookup(const std::string &op) {
      if (op == "+")
        return METHOD_ADD;
      else if (op == "-")
        return METHOD_SUBTRACT;
      else if (op =="*")
        return METHOD_MULTIPLY;
      else if (op =="/")
        return METHOD_DIVIDE;
      else if (op ==">=")
        return METHOD_GEQ;
      else if (op ==">")
        return METHOD_GT;
      else if (op =="<=")
        return METHOD_LEQ;
      else if (op =="<")
        return METHOD_LT;
      throw UnknownBinOpException(op);
    }
  };

}


namespace std {
  template<>
  struct hash<std::pair<std::string, bool>> {
    inline size_t operator()(const std::pair<std::string, bool> &v) const {
      std::hash<bool> bool_hasher;
      std::hash<std::string> string_hasher;
      return string_hasher(v.first) ^ bool_hasher(v.second);
    }
  };
}

#endif //PROJECT02_COMPILER_UTILS_H
