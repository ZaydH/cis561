//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <map>

#include "ASTNode.h"

#ifndef PROJECT02_QUACK_METHODS_H
#define PROJECT02_QUACK_METHODS_H

namespace Quack {

  class Class;

  class Method {
   public:
    typedef MapContainer<Method> Container;

    Method(const std::string &name, const std::string &return_type, AST::Block* block)
      : name_(name), return_type_name_(return_type), block_(block) {};

    ~Method() {
      delete block_;
    };
    /** Name of the method */
    std::string name_;

   private:
    /** Name of the return type of the method (if any) */
    std::string return_type_name_;
    /** Statements (if any) to perform in method */
    AST::Block* block_;
  };
}

#endif //PROJECT02_QUACK_METHODS_H
