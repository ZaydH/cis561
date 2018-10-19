//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <map>
#include <cstring>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "container_templates.h"
#include "quack_methods.h"
#include "quack_field.h"
#include "keywords.h"

#ifndef PROJECT02_QUACK_CLASSES_H
#define PROJECT02_QUACK_CLASSES_H

namespace Quack {

  class Class {
   public:
    class Container : public MapContainer<Class> {
     public:
      /**
       * Singleton object containing all of the class information.
       *
       * @return Pointer to a set of classes.
       */
      static Container* singleton();
      /**
       * Empty all stored classes in the singleton.
       */
      static void reset() {
        Container* classes = singleton();
        for (auto &pair : *classes)
          delete pair.second;
        classes->clear();
      }
      /**
       * Prints the user defined classes only.
       *
       * @param indent_depth Depth to tab the contents.
       */
      const void print_original_src(unsigned int indent_depth = 0) {
        auto print_class = new Container();

        for (const auto &pair : objs_) {
          if (pair.first == CLASS_OBJ || pair.first == CLASS_INT
              || pair.first == CLASS_STR || pair.first == CLASS_BOOL)
            continue;
          print_class->add(pair.second);
        }
        print_class->MapContainer<Class>::print_original_src_(indent_depth, "\n\n");
      }

      Container(Container const&) = delete;       // Don't Implement
      Container& operator=(Container const&) = delete;  // Don't implement
     private:
      /**
       * Private constructor since it is a singleton.
       */
      Container() : MapContainer<Class>() {};          // Don't implement
    };

    Class(char* name, char * super_type, Param::Container* params,
          AST::Block* constructor, Method::Container* methods)
      : name_(name), super_type_name_(strcmp(super_type, "") == 0 ? CLASS_OBJ : super_type),
        super_(OBJECT_NOT_FOUND), constructor_params_(params), constructor_(constructor),
        methods_(methods) {

      fields_ = new Field::Container();

//      Container* classes = Container::singleton();
//      if (classes->exists(name))
//        throw("Duplicate class named \"" + name_ + "\"");

      if (name_ == CLASS_NOTHING)
        throw std::runtime_error("Invalid class name \"" + name_ + "\"");
    }
    /**
     * Clear all dynamic memory in the object.
     */
    ~Class() {
      delete constructor_;
      delete constructor_params_;
      delete methods_;
      delete fields_;
    }
    /**
     * Checks if this class (or any of its super classes) has the specified method.
     *
     * @param name Name of the method to check
     * @return True if the class has a method with the specified name
     */
    bool has_method(const std::string &name) {
      if (methods_->exists(name))
        return true;
      if (super_)
        return super_->has_method(name);
      return false;
    }
    /**
     * Checks all classes for any cyclical inheritance.
     */
    static void check_cyclic_inheritance() {
      for (auto & class_pair : *Container::singleton()) {
        Class* quack_class = class_pair.second;

        std::vector<Class*> super_list; // Compiler needs this because cant pass a temp by reference
        Class* base_parent = quack_class->has_no_cyclic_inheritance(super_list);

        if (base_parent != BASE_CLASS) {
          throw std::runtime_error("Class " + quack_class->name_
                                   + " has a cyclic dependency with class \""
                                   + base_parent->name_ + "\"");
        }
      }
    }
    /**
     * Used to check for cyclical class inheritance.  When classes are correctly configured,
     * each class eventually points back (through the Object class) back to a single common class.
     * If there is a cyclic dependency, the current class dependency will eventually appear in the
     * \p all_super list built via recursive calls.
     *
     * @param all_super All super classes observed so far.
     * @return true if the class has no cyclic dependencies.
     */
    Class* has_no_cyclic_inheritance(std::vector<Class *> &all_super) {
      if (super_ == BASE_CLASS)
        return BASE_CLASS;

      // If the class appears in a super, then there is a cycle
      if (std::find(all_super.begin(), all_super.end(), this) != all_super.end())
        return this;

      all_super.emplace_back(this);
      return super_->has_no_cyclic_inheritance(all_super);
    }
    /**
     * Check if the class is of the specified type.
     *
     * @param name Name of the type
     * @return True if the class is of the specified type.
     */
    bool is_type(const std::string &name) {
      if (name_ == name)
        return true;
      if (super_)
        return super_->is_type(name);
      return false;
    }
    /**
     * Performs an initial type check and configuration for the class's super class as well
     * as for the method parameters, return types, and constructor parameters.
     */
    void initial_type_check() {
      configure_super_class();

      configure_method_params(*constructor_params_);
      for (auto &method_pair : *methods_) {
        Method * method = method_pair.second;
        configure_method_params(*method->params_);

        // Check and configure the method return type.
        if (method->return_type_name_ == CLASS_NOTHING)
          method->return_type_ = BASE_CLASS;
        Class *return_type = Container::singleton()->get(method->return_type_name_);
        if (return_type == OBJECT_NOT_FOUND) {
          throw std::runtime_error("Class: " + this->name_ + ", method " + method->name_
                                   + ", unknown return type \"" + method->return_type_name_ + "\"");
        }
        method->return_type_ = return_type;
      }
    }
    /** Name of the class */
    const std::string name_;
    /**
     * Debug function used to print a representation of the original quack source code
     * used to visualize the AST.
     *
     * @param indent_depth Depth to indent the generated code. Used for improved readability.
     */
    void print_original_src(unsigned int indent_depth) {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << indent_str << KEY_CLASS << " " << name_ << "(";

      constructor_params_->print_original_src(0);
      std::cout << ")";
      if (!super_type_name_.empty())
        std::cout << " " << KEY_EXTENDS << " " << super_type_name_;
      std::cout << " {\n";

      constructor_->print_original_src(indent_depth + 1);
      if (!constructor_->empty() && !methods_->empty()) {
        std::cout << "\n\n";
      }
      methods_->print_original_src(indent_depth + 1);

      std::cout << "\n" << indent_str << "}";
    }
   private:
    /**
     * Configures the super class pointer for the object.
     */
    void configure_super_class() {
      // Object is the top of the object hierarchy so handle specially.
      if (this->name_ == CLASS_OBJ) {
        super_ = BASE_CLASS;
        return;
      }

      std::string super_name = (super_type_name_.empty()) ? CLASS_OBJ : super_type_name_;

      Container* classes = Container::singleton();
      if (!classes->exists(super_type_name_)) {
        throw std::runtime_error("For class, \"" + name_ + "\", unknown super class: "
                                 + super_type_name_);
      }
      super_ = classes->get(super_name);
    }
    /**
     * Basic configuration of method parameters including verify no parameter has type nothing
     * and then links the parameters to their associated types.
     *
     * @param params Set of parameters.
     */
    void configure_method_params(Param::Container &params) {
      for (auto &param : params) {
        if (param->type_name_ == CLASS_NOTHING) {
          throw std::runtime_error("Parameter " + param->name_ + " cannot have type \""
                                   + CLASS_NOTHING + "\"");
        }

        Class* type_class = Container::singleton()->get(param->type_name_);
        if (type_class == OBJECT_NOT_FOUND) {
          throw std::runtime_error("Parameter " + param->name_ + " has undefined type \""
                                   + param->type_name_ + "\"");
        }
        param->type_class_ = type_class;
      }
    }
    /** Name of the super class of this type */
    const std::string super_type_name_;
    /** Pointer to the super class of this class. */
    Class *super_;
    Param::Container* constructor_params_;
    /** Statements in the constructor */
    AST::Block* constructor_;
   protected:
    /** All methods supported by the class */
    Method::Container* methods_;
    /** Name of all fields in the class */
    Field::Container* fields_;
    /**
     * Used to add binary operation methods to the a class.  Only used for base classes
     * like Obj, Boolean, Integer, etc.
     *
     * @param method_name
     * @param return_type
     * @param param_type
     */
    void add_binop_method(const std::string &method_name, const std::string &return_type,
                          const std::string &param_type) {

      auto params = new Param::Container();
      params->add(new Param(FIELD_OTHER_LIT_NAME, param_type));

      // ToDo Decide on the binary operations AST block
      methods_->add(new Method(method_name, return_type, params, new AST::Block()));
    }

    void add_unary_op_method(const std::string &method_name, const std::string &return_type) {
      auto params = new Param::Container();

      // ToDo Decide on the binary operations AST block
      methods_->add(new Method(method_name, return_type, params, new AST::Block()));
    }
  };

  class ObjectClass : public Class {
   public:
    ObjectClass()
        : Class(strdup(CLASS_OBJ), strdup(""), new Param::Container(),
                new AST::Block(), new Method::Container()) {
      add_base_methods();
    }
    /**
     * Create the print and equality methods for a base Object.
     */
    void add_base_methods() {
      add_binop_method(METHOD_EQUALITY, CLASS_OBJ, CLASS_BOOL);
      add_unary_op_method(METHOD_PRINT, CLASS_NOTHING);
    }
  };

  /**
   * Represents all literal objects (e.g., integer, boolean, string).  Encapsulated in this
   * subclass to standardize some value information.
   */
  class LitClass : public Class {
   public:
    explicit LitClass(char* name)
            : Class(strdup(name), strdup(CLASS_OBJ), new Param::Container(),
                    new AST::Block(), new Method::Container()) { }
  };

  class IntList : public LitClass {
   public:
    IntList() : LitClass(strdup(CLASS_INT)) {
      add_binop_method(METHOD_ADD, CLASS_INT, CLASS_INT);
      add_binop_method(METHOD_SUBTRACT, CLASS_INT, CLASS_INT);
      add_binop_method(METHOD_MULTIPLY, CLASS_INT, CLASS_INT);
      add_binop_method(METHOD_DIVIDE, CLASS_INT, CLASS_INT);

      add_binop_method(METHOD_EQUALITY, CLASS_INT, CLASS_BOOL);
      add_binop_method(METHOD_LEQ, CLASS_INT, CLASS_BOOL);
      add_binop_method(METHOD_LT, CLASS_INT, CLASS_BOOL);
      add_binop_method(METHOD_GEQ, CLASS_INT, CLASS_BOOL);
      add_binop_method(METHOD_GT, CLASS_INT, CLASS_BOOL);
    }
  };

  class StringLit : public LitClass {
   public:
    StringLit() : LitClass(strdup(CLASS_STR)) {
      add_binop_method(METHOD_ADD, CLASS_STR, CLASS_STR);

      add_binop_method(METHOD_EQUALITY, CLASS_STR, CLASS_STR);
      add_binop_method(METHOD_LEQ, CLASS_STR, CLASS_BOOL);
      add_binop_method(METHOD_LT, CLASS_STR, CLASS_BOOL);
      add_binop_method(METHOD_GEQ, CLASS_STR, CLASS_BOOL);
      add_binop_method(METHOD_GT, CLASS_STR, CLASS_BOOL);
    }
  };

  class BooleanLit : public LitClass {
   public:
    BooleanLit() : LitClass(strdup(CLASS_BOOL)) {
      add_binop_method(METHOD_EQUALITY, CLASS_BOOL, CLASS_BOOL);
    }
  };
}

#endif //PROJECT02_QUACK_CLASSES_H
