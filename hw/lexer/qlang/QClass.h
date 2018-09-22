//
// Created by Zayd Hammoudeh on 9/22/18.
//

#ifndef AST_QCLASS_H
#define AST_QCLASS_H

#include <cstdlib>
#include <string>

namespace AST {
  typedef std::map<std::string, AST::QuackClass*> ClassesContainer;
  typedef std::map<std::string, AST::QuackClass::Method*> MethodsContainer;

  class QuackClass {
   public:
    class Method {
     public:
      /** Name of the method */
      const std::string name_;
    };

    explicit QuackClass(std::string &name) : name_(name) {
      methods_ = nullptr;
    }
    /**
     * Deletes the methods_ information since those are dynamically allocated
     * by the parser.
     */
    ~QuackClass() {
      if(methods_) {
        for (auto const& pair : *methods_)
          delete pair.second();
        delete methods_;
      }
    }
    /**
     * Adds methods to the class.
     *
     * This function assumes that no methods have been attached to the class yet.
     * @param methods New collections of methods.
     */
    void add_methods(MethodsContainer* methods) {
      assert(!methods_); // Do not ever allow double allocating the methods
      methods_ = methods;
    }


    const std::string name_;
   private:
    /** Stores all of the class' methods. */
    MethodsContainer* methods_;
  };
}

#endif //AST_QCLASS_H
