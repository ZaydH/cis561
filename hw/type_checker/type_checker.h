#ifndef PROJECT02_QUACK_TYPE_CHECKER_H
#define PROJECT02_QUACK_TYPE_CHECKER_H

#include "quack_classes.h"
#include "initialized_list.h"

namespace Quack {

  class TypeChecker {
   public:
    TypeChecker() = default;

    void run() {
      perform_initial_checks();

      Class::check_cyclic_inheritance();

      perform_initialized_before_use_check();
    }

    void perform_initial_checks() {
      for (auto &class_pair : *Class::Container::singleton()) {
        Class * q_class = class_pair.second;
        q_class->initial_type_check();
      }
    }

    void perform_initialized_before_use_check() {
      InitializedList starter_list;
      starter_list.add("this");
      for (auto &class_pair : *Class::Container::singleton()) {
        Class * q_class = class_pair.second;

        for (auto &method_pair : *q_class->methods_) {
          Method * method = method_pair.second;
          InitializedList init_list = starter_list;
          method->block_->check_initialize_before_use(init_list);
        }
        InitializedList init_list = starter_list;
        q_class->constructor_->check_initialize_before_use(init_list);
      }
    }
  };
}

#endif //PROJECT02_QUACK_TYPE_CHECKER_H
