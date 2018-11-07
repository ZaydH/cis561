#ifndef PROJECT02_QUACK_TYPE_CHECKER_H
#define PROJECT02_QUACK_TYPE_CHECKER_H

#include "quack_classes.h"
#include "quack_program.h"
#include "initialized_list.h"

namespace Quack {
  class TypeChecker {
   public:
    TypeChecker() = default;

    void run(Program* prog) {
      perform_initial_checks();

      Class::check_well_formed_hierarchy();

      // ToDo manage what to do with an initialize before use failure
      perform_initialized_before_use_check(prog);
    }

    void perform_initial_checks() {
      for (auto &class_pair : *Class::Container::singleton()) {
        Class * q_class = class_pair.second;
        q_class->initial_type_check();
      }
    }

    void perform_initialized_before_use_check(Program* prog) {
      // Class type checker
      for (auto &class_pair : *Class::Container::singleton()) {
        Class * q_class = class_pair.second;

        perform_constructor_initialize_before_use(q_class);

        for (auto &method_pair : *q_class->methods_) {
          Method * method = method_pair.second;

          InitializedList init_list;
          add_params_to_initialized_list(init_list, method->params_);
          method->block_->check_initialize_before_use(init_list, nullptr);
        }
      }

      // Verifies the main block
      InitializedList empty_list;
      prog->block_->check_initialize_before_use(empty_list, nullptr);
    }

    /**
     * Performs an initialize before use check on the Quack class.  It also adds fields to the
     * Field::Container() for the class.
     *
     * @param q_class Class whose constructor is being checked
     */
    void perform_constructor_initialize_before_use(Quack::Class * q_class) {
      // ToDo remove constructor skip
      std::string name = q_class->name_;
      if (name == CLASS_OBJ || name == CLASS_INT || name == CLASS_BOOL || name == CLASS_STR)
        return;

      InitializedList init_list;
      add_params_to_initialized_list(init_list, q_class->constructor_params_);

      InitializedList * all_inits = new InitializedList();
      q_class->constructor_->check_initialize_before_use(init_list, all_inits);

      if (init_list.count() != all_inits->count())
        throw("Constructor for class " + q_class->name_ + " does not initialize on all paths");
      delete all_inits;

      for (const auto &var_info : init_list.vars_) {
        if (!var_info.second)
          continue;
        q_class->fields_->add_by_name(var_info.first);
      }
    }
    /**
     * Adds the parameters to the initialized this.  This is generally used before a function
     * call
     * @param params
     */
    static void add_params_to_initialized_list(InitializedList &init_list,
                                               const Param::Container * params) {
      for (const auto &param : *params)
        init_list.add(param->name_, false);
    }
  };
}

#endif //PROJECT02_QUACK_TYPE_CHECKER_H
