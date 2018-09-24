//
// Created by Zayd Hammoudeh on 9/22/18.
//

#ifndef AST_QCLASS_H
#define AST_QCLASS_H

#include <map>
#include <string>
#include <set>

#include "keywords.h"
#include "ASTNode.h"

namespace AST {

  class QuackClass {
   public:
    /** Used to store multiple QuackClass objects */
    typedef std::map<std::string, AST::QuackClass*> Container;

    class Parameter {
     public:
      /** Stores multiple parameters */
      typedef std::map<std::string, QuackClass::Parameter*> Container;

      Parameter(char* name, char* type_name)
          : name_(name), type_name_(type_name) {
        // ToDo add a way to link the types
        type_ = nullptr;
      }

      const std::string name_;
     private:
      /** Name of the type */
      const std::string type_name_;
      /** Link to the type for the parameter. */
      const QuackClass* type_;
    };

    /**
     * Defines a class method.
     */
    class Method {
     public:
      typedef std::map<std::string, AST::QuackClass::Method*> Container;

      explicit Method(const char* name, const QuackClass::Parameter::Container* params,
                      const char* return_type, const AST::Block* block)
        : name_(name), params_(params), return_type_name_(return_type), block_(block) {
      }
      /**
       * Clear the memory for all parameters and the instructions
       */
      ~Method() {
        if(params_)
          for (const auto &param_pair: *params_)
            delete param_pair.second;
        delete params_;

        delete block_;
      }

      /** Name of the method */
      const std::string name_;
      /** Input parameters */
      const QuackClass::Parameter::Container* params_;
      // ToDo define how the return type will be stored
      const std::string return_type_name_;
      /** AST of method instructions */
      const AST::Block* block_;
    };

    explicit QuackClass(const char* name, const char* super,
                        const QuackClass::Parameter::Container* constructor_params)
        : name_(name), constructor_params_(constructor_params) {
      assert(!is_reserved_class_name(name_));

      methods_ = nullptr;
      constructor_ = nullptr;
      // Configures the super class
      if (!super || std::strcmp(super, KEY_OBJ) == 0) {
        // ToDo Extends Obj
      } else {
        // ToDo Custom class
      }
      super_ = nullptr;
    }
    /**
     * Deletes the methods and constructor information since those are dynamically allocated
     * by the parser.
     */
    ~QuackClass() {
      delete constructor_;

      if (methods_)
        for (auto const& pair : *methods_)
          delete pair.second;
      delete methods_;

      if (constructor_params_)
        for (auto const& pair : *constructor_params_)
          delete pair.second;
      delete constructor_params_;
    }
    /**
     * Gets method with the specified name from the class itself or any of its parent
     * classes.  If the method does not exist, then nullptr is returned.
     *
     * @param method_name Name of the method to extract.
     * @return Pointer to the method.
     */
    QuackClass::Method* get_method(const std::string &method_name) {
      QuackClass::Method::Container::iterator itr;
      itr = methods_->find(method_name);
      if (itr != methods_->end())
        return itr->second;

      // See if the method is inherited from a parent
      // ToDo finalize checking super_ once obj is determined
      if (super_)
        return super_->get_method(method_name);
      return nullptr;
    }

    /**
     * Adds methods to the class.
     *
     * This function assumes that no methods have been attached to the class yet.
     * @param methods New collections of methods.
     */
    void add_methods(QuackClass::Method::Container* methods) {
      assert(!methods_); // Do not ever allow reallocating the methods
      methods_ = methods;
    }
    /**
     * Adds the constructor to the class.
     *
     * @param constructor Constructor for the par
     */
    void add_constructor(AST::Block* constructor) {
      assert(!constructor_); // Cannot overwrite the constructor
      constructor_ = constructor;
    }
    /**
     * Check whether this class has the s
     *
     * @param parent_name_ Name of the parent class.
     * @return
     */
    bool has_parent(std::string &parent_name_) {
      // ToDo confirm how to handle Obj and parent class
      if (super_) {
        if (super_->name_ == parent_name_)
          return true;
        return super_->has_parent(parent_name_);
      }

      return parent_name_ == KEY_OBJ;
    }

    // ToDo Add support for PRINT and STR

    const std::string name_;
   private:
    /**
     *
     * @param str String to check if part of a reserved class name
     * @return True if the passed string is a reserved class name
     */
    static bool is_reserved_class_name(const std::string &str) {
      return reserved_class_names_.find(str) != reserved_class_names_.end();
    }
    /** Stores all of the class' methods. */
    QuackClass::Method::Container* methods_;
    AST::Block* constructor_;
    /** Type of the super class */
    QuackClass * super_;
    /** Parameters supplied to the constructor */
    const QuackClass::Parameter::Container* constructor_params_;
    /** Define illegal names for fields, variables and methods */
    static std::set<std::string> reserved_class_names_;
  };
}

#endif //AST_QCLASS_H
