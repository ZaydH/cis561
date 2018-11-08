#ifndef PROJECT02_QUACK_TYPE_CHECKER_H
#define PROJECT02_QUACK_TYPE_CHECKER_H

#include "quack_classes.h"
#include "quack_program.h"
#include "initialized_list.h"
#include "symbol_table.h"

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
    /**
     * Performs all initialize before use tests for all Quack classes.  The function also checks
     * that all super fields are tested as well.
     *
     * @param prog Quack program being type checked.
     */
    void perform_initialized_before_use_check(Program* prog) {
      // Type check constructor first since the fields are need for the method checks
      // and for the super class field checks.
      for (auto &class_pair : *Class::Container::singleton()) {
        Class *q_class = class_pair.second;
        perform_constructor_initialize_before_use(q_class);
      }

      all_super_fields_initialized();

      // Checks all methods other than the constructor
      for (auto &class_pair : *Class::Container::singleton()) {
        Class *q_class = class_pair.second;

        InitializedList param_list = InitializedList();
        add_params_to_initialized_list(param_list, q_class->constructor_params_, true);

        for (auto &method_pair : *q_class->methods_) {
          Method * method = method_pair.second;

          InitializedList init_list(param_list);
          add_params_to_initialized_list(init_list, method->params_);
          method->block_->check_initialize_before_use(init_list, nullptr, true);
        }
      }

      // Verifies the main block
      InitializedList empty_list;
      prog->block_->check_initialize_before_use(empty_list, nullptr, false);
    }

    /**
     * Performs an initialize before use check on a Quack class's constructor.  No other methods
     * (if any) from the Quack class are checked in this method.  The method does populate the
     * Field::Container() for the class.
     *
     * @param q_class Class whose constructor is being checked
     */
    void perform_constructor_initialize_before_use(Quack::Class * q_class) {
      std::string name = q_class->name_;

      InitializedList init_list;
      add_params_to_initialized_list(init_list, q_class->constructor_params_);

      auto all_inits = new InitializedList(init_list);
      q_class->constructor_->check_initialize_before_use(init_list, all_inits, true);

      if (init_list.count() != all_inits->count())
        throw ("Constructor for class " + q_class->name_ + " does not initialize on all paths");
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
                                               const Param::Container * params,
                                               bool is_field = false) {
      for (const auto &param : *params)
        init_list.add(param->name_, is_field);
    }
    /**
     * This verifies that all fields of a superclass are initialized in the subclass.  The overall
     * algorithm only checks each class with its parent.  If all parent pairwise tests pass,
     * then the field structure is valid.
     *
     * @return True if all fields of the super class are initialized in the subclass.
     */
    bool all_super_fields_initialized() {
      Class::Container* all_classes = Class::Container::singleton();

      for (auto &class_pair : *all_classes) {
        Class *q_class = class_pair.second;
        if (!q_class->is_user_class())
          continue;

        if (q_class->super_ == all_classes->get(CLASS_OBJ))
          continue;

        if (!q_class->fields_->is_super_set(q_class->super_->fields_))
          throw MissingSuperFieldsException(q_class->name_);
      }

      return true;
    }

    bool type_inference(Program* prog) {
      assert(false);

      for (auto &class_info : *Quack::Class::Container::singleton()) {
        Quack::Class * q_class = class_info.second;
        // Test constructor first
        function_type_inference(q_class, q_class->constructor_, q_class->constructor_params_);
        // Perform type inference on each method
        for (auto &method_info : *q_class->methods_) {
          auto * method = method_info.second;
          function_type_inference(q_class, method->block_, method->params_);
        }
      }

      // Performs inference on the main function
      function_type_inference(nullptr, prog->block_, nullptr);
    }

    void function_type_inference(Quack::Class * q_class, AST::Block* block,
                                 Param::Container* params) {
      Symbol::Table st;
      // Add any field variables to the symbol table.
      if (q_class && q_class->fields_ && !q_class->fields_->empty())
        st.add_fields(q_class->fields_);
      // Add any parameters to the symbol table
      if (params && !params->empty())
        st.add_params(params);

      assert(false);
    }
  };
}

#endif //PROJECT02_QUACK_TYPE_CHECKER_H
