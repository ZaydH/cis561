//
// Created by Zayd Hammoudeh on 10/4/18.
//

#ifndef PROJECT02_QUACK_CLASSES_H
#define PROJECT02_QUACK_CLASSES_H

#include <map>
#include <cstring>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "container_templates.h"
#include "quack_method.h"
#include "quack_field.h"
#include "keywords.h"

namespace Quack {

  class TypeChecker;

  class Class {
    friend class TypeChecker;
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
      const void print_original_src(unsigned int indent_depth) override {
        auto print_class = new Container();

        for (const auto &pair : objs_) {
          if (pair.first == CLASS_OBJ || pair.first == CLASS_INT
              || pair.first == CLASS_STR || pair.first == CLASS_BOOL)
            continue;
          print_class->add(pair.second);
        }
        print_class->MapContainer<Class>::print_original_src_(indent_depth, "\n\n");
      }
      /**
       * Static accessor to get the Integer class.
       *
       * @return Integer class reference.
       */
      static Class* Int() {
        return singleton()->get(CLASS_INT);
      }
      /**
       * Static accessor to get the Boolean class.
       *
       * @return Boolean class reference.
       */
      static Class* Bool() {
        return singleton()->get(CLASS_BOOL);
      }
      /**
       * Static accessor to get the String class.
       *
       * @return String class reference.
       */
      static Class* Str() {
        return singleton()->get(CLASS_STR);
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
        super_(OBJECT_NOT_FOUND), methods_(methods) {

      fields_ = new Field::Container();

      // Constructor has same function name as the class
      constructor_ = new Method(name, this->name_, params, constructor);
      constructor_->return_type_ = this;

//      Container* classes = Container::singleton();
//      if (classes->exists(name))
//        throw("Duplicate class named \"" + name_ + "\"");

      if (name_ == CLASS_NOTHING)
        throw ParserException("Invalid class name \"" + name_ + "\"");
    }
    /**
     * Clear all dynamic memory in the object.
     */
    virtual ~Class() {
      delete constructor_;
      delete methods_;
      delete fields_;
    }
    /**
     * Checks whether this class (or any of its super classes) has a field with the specified name.
     *
     * @param name Field name
     * @return True if the field exists
     */
    bool has_field(const std::string &name) {
      if (fields_->exists(name) || (super_ && super_->has_field(name)))
        return true;
      return false;
    }
    /**
     * Checks if this class (or any of its super classes) has the specified method.
     *
     * @param name Name of the method to check
     * @return True if the class has a method with the specified name
     */
    bool has_method(const std::string &name) {
      if (methods_->exists(name) || (super_ && super_->has_method(name)))
        return true;
      return false;
    }
    /**
     * Accessor for a method by the method's name.
     *
     * @param name Method's name
     * @return Method pointer.
     */
    Method* get_method(const std::string &name) {
      if (methods_->exists(name))
        return methods_->get(name);
      if (super_ == BASE_CLASS)
        return OBJECT_NOT_FOUND;
      return super_->get_method(name);
    }
    /**
     * Checks all classes for any cyclical inheritance.
     */
    static void check_well_formed_hierarchy() {
      for (auto & class_pair : *Container::singleton()) {
        Class* quack_class = class_pair.second;

        std::vector<Class*> super_list; // Compiler needs this because cant pass a temp by reference
        Class* base_parent = quack_class->has_no_cyclic_inheritance(super_list);

        if (base_parent != BASE_CLASS) {
          std::string str = "Class " + quack_class->name_ + " has a cyclic inheritance";
          throw CyclicInheritenceException("CyclicInheritance", str);
        }
      }

      // Check that the return type of inherited methods is subtype of super method
      for (auto & class_pair : *Container::singleton()) {
        Quack::Class * q_class = class_pair.second;
        if (q_class->super_ == BASE_CLASS)
          continue;

        for (auto &method_info : *q_class->methods_) {
          Method * method = method_info.second;
          if (!q_class->super_->has_method(method->name_))
            continue;

          Quack::Class * method_rtype = method->return_type_;
          Quack::Class * super_rtype = q_class->super_->get_method(method->name_)->return_type_;

          if (!method_rtype->is_subtype(super_rtype))
            throw InheritedMethodReturnTypeException(q_class->name_, method->name_);
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
     * Perform least common ancestor determination on the implicit class and the \p other class.
     *
     * @param other Another
     * @return
     */
    Class* least_common_ancestor(Class * other) {
      return Class::least_common_ancestor(this, other);
    }
    /**
     * Helper function used to find the least common ancestor of two classes.  If there is no
     * common ancestor, the function
     * @param c1 First class to compare
     * @param c2 Second class to compare
     * @return Class shared by the two classes in the hierarchy
     */
    static Class* least_common_ancestor(Class* c1, Class* c2) {
      assert(c1 != nullptr && c2 != nullptr);
      if (c1 == c2)
        return c1;

      std::vector<std::vector<Class*>> class_paths(2);
      Class* classes[] = {c1, c2};
      // Build the list of paths
      for (int i = 0; i < 2; i++) {
        Class * q_class = classes[i];
        do {
          class_paths[i].emplace_back(q_class);
          q_class = q_class->super_;
        } while (q_class != BASE_CLASS);
      }

      // iterate through the lists in reverse to find last class both share
      unsigned long size0 = class_paths[0].size();
      unsigned long size1 = class_paths[1].size();
      auto min_len = std::min<unsigned long>(size0, size1);
      Class * last_shared = BASE_CLASS;
      for (unsigned long i = 1; i <= min_len; i++) {
        if (class_paths[0][size0 - i] != class_paths[1][size1 - i]) {
          assert(last_shared != BASE_CLASS);
          return last_shared;
        }

        last_shared = class_paths[0][size0 - i];
      }
      // One is a proper subset of the other
      return last_shared;
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
     * Determines if the implicit class is a subtype of the specified type.
     *
     * @param other_type Type to check whether this class is a subtype
     * @return True if this class is a subtype of \p other_type.
     */
    bool is_subtype(Class * other_type) {
      if (other_type == BASE_CLASS)
        return true;

      Class * super = this;
      while (super != BASE_CLASS) {
        if (super == other_type)
          return true;
        super = super->super_;
      }
      return false;
    }
    /**
     * Performs an initial type check and configuration for the class's super class as well
     * as for the method parameters, return types, and constructor parameters.
     */
    void initial_type_check() {
      configure_super_class();

      configure_method_params(*constructor_->params_);
      for (auto &method_pair : *methods_) {
        Method * method = method_pair.second;
        configure_method_params(*method->params_);

        // Check and configure the method return type.
        if (method->return_type_name_ == CLASS_NOTHING) {
          method->return_type_ = BASE_CLASS;
        } else {
          method->return_type_ = Container::singleton()->get(method->return_type_name_);
          if (method->return_type_ == OBJECT_NOT_FOUND) {
            throw UnknownTypeException("Class: " + this->name_ + ", method " + method->name_
                                     + ", unknown return type \"" + method->return_type_name_ +
                                     "\"");
          }
        }
      }
    }
    /** Name of the class */
    const std::string name_;
//    /**
//     * Accessor for all the methods in the class.
//     * @return Methods in the class.
//     */
//    inline const Method::Container* methods() const { return methods_; }
    /**
     * Debug function used to print a representation of the original quack source code
     * used to visualize the AST.
     *
     * @param indent_depth Depth to indent the generated code. Used for improved readability.
     */
    void print_original_src(unsigned int indent_depth) {
      std::string indent_str = std::string(indent_depth, '\t');
      std::cout << indent_str << KEY_CLASS << " " << name_ << "(";

      constructor_->params_->print_original_src(0);
      std::cout << ")";
      if (!super_type_name_.empty())
        std::cout << " " << KEY_EXTENDS << " " << super_type_name_;
      std::cout << " {\n";

      constructor_->print_original_src(indent_depth + 1);
      if (!constructor_->block_->empty() && !methods_->empty()) {
        std::cout << "\n\n";
      }
      methods_->print_original_src(indent_depth + 1);

      std::cout << "\n" << indent_str << "}";
    }
    /**
     * Base classes are built into the Quack language and include Boolean, Integer, String,
     * and Object.  User classes are NOT built into the Quack language by default and are
     * defined by the user..
     *
     * @return True if the class is abase class.
     */
    virtual bool is_user_class() const { return true; }
    /**
     * Accessor for the class constructor.
     *
     * @return Constructor method
     */
    Method* get_constructor() { return constructor_; }
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
        std::string msg = "For class, \"" + name_ + "\", unknown super class: " + super_type_name_;
        throw ClassHierarchyException("UnknownSuper", msg);
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
          std::string msg = "Parameter " + param->name_ + " cannot have type \"" CLASS_NOTHING "\"";
          throw ClassHierarchyException("NothingParam", msg);
        }

        Class* type_class = Container::singleton()->get(param->type_name_);
        if (type_class == OBJECT_NOT_FOUND)
          throw UnknownTypeException(param->type_name_);
        param->type_ = type_class;
      }
    }
    /** Name of the super class of this type */
    const std::string super_type_name_;
    /** Pointer to the super class of this class. */
    Class *super_;
    /** Statements in the constructor */
    Method* constructor_;
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

      // ToDo Decide on binop code generation strategy
      methods_->add(new Method(method_name, return_type, params, new AST::Block()));
      methods_->get(method_name)->init_list_ = new InitializedList();
    }

    void add_unary_op_method(const std::string &method_name, const std::string &return_type) {
      auto params = new Param::Container();

      // ToDo Decide on binop code generation strategy
      methods_->add(new Method(method_name, return_type, params, new AST::Block()));
    }
  };

  struct ObjectClass : public Class {
    ObjectClass()
        : Class(strdup(CLASS_OBJ), strdup(""), new Param::Container(),
                new AST::Block(), new Method::Container()) {
      add_base_methods();
    }
    /**
     * Create the print and equality methods for a base Object.
     */
    void add_base_methods() {
      add_binop_method(METHOD_EQUALITY, CLASS_BOOL, CLASS_OBJ);
      // Unary op ok to use since function takes no args
      add_unary_op_method(METHOD_PRINT, CLASS_NOTHING);
      add_unary_op_method(METHOD_STR, CLASS_STR);
    }
    /**
     * Object class is a base class in Quack so this function always returns true since if it is
     * a base class, it cannot be a user class.
     *
     * @return True always.
     */
    bool is_user_class() const override { return false; }
  };

  /**
   * Represents all literal objects (e.g., integer, boolean, string).  Encapsulated in this
   * subclass to standardize some value information.
   */
  struct PrimitiveClass : public Class {
    explicit PrimitiveClass(char* name)
            : Class(strdup(name), strdup(CLASS_OBJ), new Param::Container(),
                    new AST::Block(), new Method::Container()) { }
    /**
    * Primitives are all base (i.e., not user) classes in Quack so this function always returns
    * true.
    *
    * @return True always.
    */
    bool is_user_class() const override { return false; }
  };

  struct IntClass : public PrimitiveClass {
    IntClass() : PrimitiveClass(strdup(CLASS_INT)) {
      add_unary_op_method(METHOD_STR, CLASS_STR);

      add_binop_method(METHOD_ADD, CLASS_INT, CLASS_INT);
      add_binop_method(METHOD_SUBTRACT, CLASS_INT, CLASS_INT);
      add_binop_method(METHOD_MULTIPLY, CLASS_INT, CLASS_INT);
      add_binop_method(METHOD_DIVIDE, CLASS_INT, CLASS_INT);

      add_binop_method(METHOD_EQUALITY, CLASS_BOOL, CLASS_INT);
      add_binop_method(METHOD_LEQ, CLASS_BOOL, CLASS_INT);
      add_binop_method(METHOD_LT, CLASS_BOOL, CLASS_INT);
      add_binop_method(METHOD_GEQ, CLASS_BOOL, CLASS_INT);
      add_binop_method(METHOD_GT, CLASS_BOOL, CLASS_INT);
    }
  };

  struct StringClass : public PrimitiveClass {
    StringClass() : PrimitiveClass(strdup(CLASS_STR)) {
      add_unary_op_method(METHOD_STR, CLASS_STR);

      add_binop_method(METHOD_ADD, CLASS_STR, CLASS_STR);

      add_binop_method(METHOD_EQUALITY, CLASS_BOOL, CLASS_STR);
      add_binop_method(METHOD_LEQ, CLASS_BOOL, CLASS_STR);
      add_binop_method(METHOD_LT, CLASS_BOOL, CLASS_STR);
      add_binop_method(METHOD_GEQ, CLASS_BOOL, CLASS_STR);
      add_binop_method(METHOD_GT, CLASS_BOOL, CLASS_STR);
    }
  };

  struct BooleanClass : public PrimitiveClass {
    BooleanClass() : PrimitiveClass(strdup(CLASS_BOOL)) {
      add_unary_op_method(METHOD_STR, CLASS_STR);

      add_binop_method(METHOD_EQUALITY, CLASS_BOOL, CLASS_BOOL);

      add_binop_method(METHOD_OR, CLASS_BOOL, CLASS_BOOL);
      add_binop_method(METHOD_AND, CLASS_BOOL, CLASS_BOOL);
    }
  };
}

#endif //PROJECT02_QUACK_CLASSES_H
