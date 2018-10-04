//
// Created by Zayd Hammoudeh on 10/4/18.
//

#include <vector>
#include <string>

#include "ASTNode.h"
#include "quack_classes.h"

#ifndef PROJECT02_QUACK_PROGRAM_H
#define PROJECT02_QUACK_PROGRAM_H

namespace Quack {

  class Program {
   public:
    explicit Program(const Class::Container *classes, const AST::Block *block) :
       classes_(classes), block_(block) {};

    ~Program() {
      delete classes_;
      delete block_;
    }

   private:
    const Class::Container *classes_;
    const AST::Block* block_;
  };
}

#endif //PROJECT02_QUACK_PROGRAM_H
