//
// Created by Zayd Hammoudeh on 9/22/18.
//

#ifndef AST_QCLASS_H
#define AST_QCLASS_H

#include <map>
#include <vector>
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
      typedef std::vector<QuackClass::Parameter*> Container;

      Parameter(char* name, char* type_name)
          : name_(name), type_name_(type_name) {
        // ToDo add a way to link the types
        type_ = nullptr;
      }
      /**
       * Add the parameter to the parameter container.
       * @param cont A parameter container
       * @param param A new parameter
       */
      static void add_to_container(Container* cont, Parameter *param) {
        assert(!container_has_param(cont, param->name_));
        cont->emplace_back(param);
      }
      /**
       * Checks whether the parameter is already in the container.
       *
       * @param cont Parameter container
       * @param param_name Name of parameter to check if already in container
       *
       * @return True if a parameter in the container has a matching name.
       */
      static bool container_has_param(Container* cont, const std::string &param_name) {
        for(const auto &param: *cont)
          if (param->name_ == param_name)
            return true;
        return false;
      }
      /**
       * Delete all parameters in the parameter container.
       *
       * @param cont Container containing parameters.
       */
      static void delete_container(const Container * cont) {
        for (const auto &param: *cont)
          delete param;
      }

      /** Prints all parameters in the container */
      static std::string print_container(const Container *container) {
        std::stringstream ss;
        bool is_first = true;
        for (const auto &param : *container){
          if (!is_first)
            ss << ", ";
          is_first = false;
          ss << param->STR();
        }
        return ss.str();
      }

      std::string STR() {
        std::stringstream ss;
        ss << name_;
        if (!type_name_.empty())
          ss << " : " << type_name_;
        return ss.str();
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
          Parameter::delete_container(params_);
        delete params_;

        delete block_;
      }

      std::string STR() {
        std::stringstream ss;
        ss << KEY_DEF << " " << name_ << "("
           << QuackClass::Parameter::print_container(params_) << ")";
        if (!return_type_name_.empty())
          ss << " : " << return_type_name_;
        ss << " {\n" << const_cast<AST::Block*>(block_)->STR() << "}\n";
        return ss.str();
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
        : name_(name), super_name_(super), constructor_params_(constructor_params) {
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
        Parameter::delete_container(constructor_params_);
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

    std::string STR() {
      std::stringstream ss;
      ss << KEY_CLASS << " " << name_ << "("
         << QuackClass::Parameter::print_container(constructor_params_) << ")";
      if (!super_name_.empty())
        ss << " extends " << super_name_;
      ss << " {\n";
      ss << constructor_->STR();
      if (methods_) {
        ss << "\n";
        for (auto const &pair : *methods_) {
          ss << pair.second->STR();
        }
      }
      ss << "}\n";
      return ss.str();
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
    const std::string super_name_;
    QuackClass * super_;
    /** Parameters supplied to the constructor */
    const QuackClass::Parameter::Container* constructor_params_;
    /** Define illegal names for fields, variables and methods */
    static std::set<std::string> reserved_class_names_;
  };


  class Program {
   public:
    Program(const QuackClass::Container * classes, ASTNode * block)
            : classes_(classes), block_(block) {}
    /**
     * Clears memory of the classes and the block of code.
     */
    ~Program() {
      if (classes_)
        for (const auto &pair : *classes_)
          delete pair.second;
      delete classes_;
      delete block_;
    }

    /** Container containing a reference to all classes */
    const QuackClass::Container * classes_;
    /** All code to execute */
    ASTNode * block_;
  };
}

#endif //AST_QCLASS_H
