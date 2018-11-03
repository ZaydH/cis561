//
// Created by Zayd Hammoudeh on 10/15/18.
//

#ifndef PROJECT02_QUACK_TYPE_CHECKER_H
#define PROJECT02_QUACK_TYPE_CHECKER_H

#include "quack_classes.h"

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

    void perform_initialized_before_use_check() {
      for (const auto &pair : *Class::Container::singleton()) {
        Quack::Class* cls = pair.second;
        for (const auto &method : cls->methods())
          method.check_initialize_before_use();
      }
    }
  };
}

#endif //PROJECT02_QUACK_TYPE_CHECKER_H
