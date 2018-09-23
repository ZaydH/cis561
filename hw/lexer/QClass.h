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

    /**
     * Defines a class method.
     */
    class Method {
     public:
      typedef std::map<std::string, AST::QuackClass::Method*> Container;

      explicit Method(const char* name, const char* return_type, const AST::Block* block)
        : name_(name), return_type_name_(return_type), block_(block) {}

      /** Name of the method */
      const std::string name_;
      // ToDo define how the return type will be stored
      const std::string return_type_name_;
      const AST::Block* block_;
    };

    explicit QuackClass(const char* name, const char* super) : name_(name) {
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
  };
}

#endif //AST_QCLASS_H
