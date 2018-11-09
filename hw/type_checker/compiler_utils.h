//
// Created by Zayd Hammoudeh on 11/7/18.
//

#ifndef PROJECT02_COMPILER_UTILS_H
#define PROJECT02_COMPILER_UTILS_H

#include <string>

#include "keywords.h"
#include "exceptions.h"
#include "symbol_table.h"

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

// Forward Declaration
namespace Quack { class Class; }

namespace TypeCheck {
  struct Settings {

    Settings() : st_(nullptr), this_class_(nullptr), return_type_(nullptr), is_constructor_(false){}

    Symbol::Table * st_;
    Quack::Class * this_class_;
    Quack::Class * return_type_;
    bool is_constructor_;
  };
}

#endif //PROJECT02_COMPILER_UTILS_H
