//
// Created by Zayd Hammoudeh on 10/15/18.
//

#include "quack_classes.h"

#ifndef PROJECT02_QUACK_TYPE_CHECKER_H
#define PROJECT02_QUACK_TYPE_CHECKER_H

namespace Quack {

  class TypeChecker {
   public:
    TypeChecker() = default;

    void run() {
      Class::Container *classes = Class::Container::singleton();

      for (auto &class_pair : *classes) {
        Class * q_class = class_pair.second;
        q_class->initial_type_check();
      }

      Class::check_cyclic_inheritance();
    }
  };
}

#endif //PROJECT02_QUACK_TYPE_CHECKER_H
