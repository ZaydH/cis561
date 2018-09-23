//
// Created by Zayd Hammoudeh on 9/22/18.
//

#ifndef AST_QCLASS_H
#define AST_QCLASS_H

#include <map>
#include <string>

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
     * Adds methods to the class.
     *
     * This function assumes that no methods have been attached to the class yet.
     * @param methods New collections of methods.
     */
    void add_methods(QuackClass::Method::Container* methods) {
      assert(!methods_); // Do not ever allow double allocating the methods
      methods_ = methods;
    }

    void add_constructor(AST::Block* constructor) {
      assert(!constructor_);
      constructor_ = constructor;
    }


    const std::string name_;
   private:
    /** Stores all of the class' methods. */
    QuackClass::Method::Container* methods_;
    AST::Block* constructor_;
    /** Type of the super class */
    QuackClass * super_;
    /** Parameters supplied to the constructor */
    const QuackClass::Parameter::Container* constructor_params_;
  };
}

#endif //AST_QCLASS_H
